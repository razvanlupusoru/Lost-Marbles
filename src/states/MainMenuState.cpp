#include <Ogre.h>
#include "MainMenuState.h"
#include "QuitGameState.h"
#include "PlayState.h"
#include "utils.h"
#include <fstream>


#include "FileSystemInfo.h"

#include <OgreBulletDynamicsRigidBody.h>
#include <Shapes/OgreBulletCollisionsSphereShape.h>
#include <Shapes/OgreBulletCollisionsTrimeshShape.h>

#include <Constraints/OgreBulletDynamicsPoint2pointConstraint.h>
#include <Utils/OgreBulletCollisionsMeshToShapeConverter.h>

using namespace Ogre;

MainMenuState MainMenuState::sMainMenuState;

MainMenuState::~MainMenuState()
{
	exit();
}

void MainMenuState::enter()
{
	mTimeElapsed = 0;
	mMouse = InputManager::getSingletonPtr()->getMouse();
	mRoot = Root::getSingletonPtr();
	mSoundMgr = SoundManager::getSingletonPtr();
	std::string soundCfgPath = utils::getLostMarblesWriteDir()+"\\Sound.cfg";
	ImprovedConfigFile soundSettings;
	//This access() command checks if the config exists as a read/writeable file
	if( access( soundCfgPath.c_str(), 6 ) != -1 ) 
	{
		soundSettings.load(soundCfgPath,"=:\t", false);
		mEffectVolume = StringConverter::parseInt(soundSettings.getSetting("Effects Volume",StringUtil::BLANK,"99"));
		mMusicVolume = StringConverter::parseInt(soundSettings.getSetting("Music Volume",StringUtil::BLANK,"25"));
		mMuted = StringConverter::parseBool(soundSettings.getSetting("Mute",StringUtil::BLANK,"No"));
	} else 
	{
		std::ofstream File(soundCfgPath.c_str());
		File.close();
		mEffectVolume=99;
		mMusicVolume=25;
		mMuted=0;
		saveSounds();
	}
	mSoundMgr->SetMute(mMuted);
	FMOD_ChannelGroup_SetVolume(mSoundMgr->mMusicChannelGroup,(float)mMusicVolume/100.0);
	FMOD_ChannelGroup_SetVolume(mSoundMgr->mEffectChannelGroup,(float)mEffectVolume/100.0);
	mVideoSettings.load(utils::getLostMarblesWriteDir()+"\\ogre.cfg","=:\t",false);
	createSounds();

	mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "MainMenuSMInstance");
	mCamera = mSceneMgr->createCamera("MainMenuCamera");
	resume(); // Creates viewport among other things
	mCamera->setAspectRatio(Real(mViewport->getActualWidth()) / Real(mViewport->getActualHeight()));
	mCamera->setNearClipDistance(1.0);
	mCamNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("menuCamNode");
	mCamNode->attachObject(mCamera);
	createScene();

	mSceneMgr->setAmbientLight(ColourValue(0.2, 0.2, 0.2));

	Light *light = mSceneMgr->createLight("Light1");
	light->setType(Light::LT_POINT);
	mCamNode->attachObject(light);
	//light->setPosition(mCamera->getPosition());
	light->setDiffuseColour(ColourValue::White);
	light->setSpecularColour(ColourValue::White);

	/* This only has to be done once in the game since MyGUI instance never changes */
	mPlatform = new MyGUI::OgrePlatform();
	mPlatform->initialise(mRoot->getAutoCreatedWindow(), mSceneMgr, "General", utils::getLostMarblesWriteDir()+"\\MyGUI.log");
	mGUI = new MyGUI::Gui();
	mGUI->initialise("core.xml",utils::getLostMarblesWriteDir()+"\\MyGUI.log");

	//These lines set the default ois and mygui mouse position to center screen
	OIS::MouseState ms = mMouse->getMouseState();
	mGUI->injectMouseMove(ms.width/2,ms.height/2,0);
	OIS::MouseState &mutableMouseState = const_cast<OIS::MouseState &>(mMouse->getMouseState());
	mutableMouseState.X.abs = ms.width/2;
	mutableMouseState.Y.abs = ms.height/2;

	DotLevelLoader loader(false);
	loader.parseLevels("game.levels", mLevels);

	std::string path = utils::getLostMarblesWriteDir()+"\\"+"Save.cfg";
	//If there was no save file, create an empty one
	//We do this here so we can avoid making this check if the player skips
	//the load screen and starts new game
	if( access( path.c_str(), 6 ) == -1 ) 
	{
		std::ofstream File(path.c_str());
		File << "[Progress]\n" << "[Total Marbles]\n";
		File.close();
	}
	initializeGUI();
}

void MainMenuState::initializeGUI()
{
	mWidgetNavigation = new MyGUI::WidgetPtr[4];

	MyGUI::OgreRenderManager::getInstance().setSceneManager(mSceneMgr);
	mGUI->setVisiblePointer(true);

	mMainMenuLayout = MyGUI::LayoutManager::getInstance().loadLayout("MainMenu.layout");
	mSettingsLayout = MyGUI::LayoutManager::getInstance().loadLayout("SettingsWindow.layout");
	mCreditsLayout = MyGUI::LayoutManager::getInstance().loadLayout("Credits.layout");
	mNewGameWindowLayout = MyGUI::LayoutManager::getInstance().loadLayout("NewGameWindow.layout");

	mGUI->findWidget<MyGUI::StaticImage>("LogoImage")->setPosition(MyGUI::IntPoint((mViewport->getActualWidth()/2 - 512 + 72),40));

	mWidgetNavigation[3] = mGUI->findWidget<MyGUI::StaticText>("ExitButton");
	mWidgetNavigation[3]->eventMouseButtonClick = MyGUI::newDelegate(this, &MainMenuState::handleExitGameButton);
	mWidgetNavigation[3]->eventMouseSetFocus = MyGUI::newDelegate(this, &MainMenuState::handleMouseSetFocusOnButton);
	mWidgetNavigation[3]->eventMouseLostFocus = MyGUI::newDelegate(this, &MainMenuState::handleMouseLostFocusOnButton);
	mWidgetNavigation[3]->setPosition(MyGUI::IntPoint((mViewport->getActualWidth()/2 - 512 + 472),566));
	
	mWidgetNavigation[1] = mGUI->findWidget<MyGUI::StaticText>("SettingsButton");
	mWidgetNavigation[1]->eventMouseButtonClick = MyGUI::newDelegate(this, &MainMenuState::handleSettingsButton);
	mWidgetNavigation[1]->eventMouseSetFocus = MyGUI::newDelegate(this, &MainMenuState::handleMouseSetFocusOnButton);
	mWidgetNavigation[1]->eventMouseLostFocus = MyGUI::newDelegate(this, &MainMenuState::handleMouseLostFocusOnButton);
	mWidgetNavigation[1]->setPosition(MyGUI::IntPoint((mViewport->getActualWidth()/2 - 512 + 428),362));

	mWidgetNavigation[0] = mGUI->findWidget<MyGUI::StaticText>("LoadGameButton");
	mWidgetNavigation[0]->eventMouseButtonClick = MyGUI::newDelegate(this, &MainMenuState::handleLoadButton);
	mWidgetNavigation[0]->eventMouseSetFocus = MyGUI::newDelegate(this, &MainMenuState::handleMouseSetFocusOnButton);
	mWidgetNavigation[0]->eventMouseLostFocus = MyGUI::newDelegate(this, &MainMenuState::handleMouseLostFocusOnButton);
	mWidgetNavigation[0]->setPosition(MyGUI::IntPoint((mViewport->getActualWidth()/2 - 512 + 400),262));

	mWidgetNavigation[2] = mGUI->findWidget<MyGUI::StaticText>("CreditsButton");
	mWidgetNavigation[2]->eventMouseButtonClick = MyGUI::newDelegate(this, &MainMenuState::handleCreditsButton);
	mWidgetNavigation[2]->eventMouseSetFocus = MyGUI::newDelegate(this, &MainMenuState::handleMouseSetFocusOnButton);
	mWidgetNavigation[2]->eventMouseLostFocus = MyGUI::newDelegate(this, &MainMenuState::handleMouseLostFocusOnButton);
	mWidgetNavigation[2]->setPosition(MyGUI::IntPoint((mViewport->getActualWidth()/2 - 512 + 438),464));

	MyGUI::WindowPtr window = mGUI->findWidget<MyGUI::Window>("SettingsWindow");
	window->eventWindowButtonPressed = MyGUI::newDelegate(this, &MainMenuState::handleCancelWindow);
	window->setPosition(MyGUI::IntPoint((mViewport->getActualWidth()/2 - 512 + 126),250));

	window = mGUI->findWidget<MyGUI::Window>("CreditsWindow");
	window->eventWindowButtonPressed = MyGUI::newDelegate(this, &MainMenuState::handleCancelWindow);
	window->setPosition(MyGUI::IntPoint((mViewport->getActualWidth()/2 - 512 + 126),136));

	MyGUI::HScrollPtr effectSlider = mGUI->findWidget<MyGUI::HScroll>("SettingsWindow/AudioTab/EffectVolumeSlider");
	effectSlider->eventScrollChangePosition = MyGUI::newDelegate(this, &MainMenuState::handleEffectSlider);
	effectSlider->setScrollPosition(mEffectVolume);

	MyGUI::HScrollPtr musicSlider = mGUI->findWidget<MyGUI::HScroll>("SettingsWindow/AudioTab/MusicVolumeSlider");
	musicSlider->eventScrollChangePosition = MyGUI::newDelegate(this, &MainMenuState::handleMusicSlider);
	musicSlider->setScrollPosition(mMusicVolume);

	mMuteCheckBox = mGUI->findWidget<MyGUI::Button>("SettingsWindow/AudioTab/MuteSoundsCheckBox");
	mMuteCheckBox->eventMouseButtonClick = MyGUI::newDelegate(this, &MainMenuState::handleMuteSounds);
	mMuteCheckBox->setStateCheck(mMuted);
	mSoundMgr->SetMute(mMuted);

	
	MyGUI::ComboBoxPtr combo = mGUI->findWidget<MyGUI::ComboBox>("SettingsWindow/VideoTab/ResolutionList");
	combo->eventComboAccept = MyGUI::newDelegate(this, &MainMenuState::handleResolution);

	combo = mGUI->findWidget<MyGUI::ComboBox>("SettingsWindow/VideoTab/AAList");
	combo->eventComboAccept = MyGUI::newDelegate(this, &MainMenuState::handleAA);
	
	MyGUI::ButtonPtr button = mGUI->findWidget<MyGUI::Button>("SettingsWindow/VideoTab/FullScreenCheckBox");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &MainMenuState::handleFullScreen);
	if(mVideoSettings.getSetting("Full Screen","Direct3D9 Rendering Subsystem").compare("Yes")==0)
		button->setStateCheck(true);
	else
		button->setStateCheck(false);

	button = mGUI->findWidget<MyGUI::Button>("SettingsWindow/VideoTab/VSyncCheckBox");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &MainMenuState::handleVSync);
	if(StringConverter::parseBool(mVideoSettings.getSetting("VSync","Direct3D9 Rendering Subsystem")))
		button->setStateCheck(true);
	else
		button->setStateCheck(false);

	mWidgetFocus = 0;
	setFocusWidget(mWidgetNavigation[mWidgetFocus]);	

	button = mGUI->findWidget<MyGUI::Button>("SettingsWindow/SaveToFileButton");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &MainMenuState::handleButtonPresses);

	button = mGUI->findWidget<MyGUI::Button>("SettingsWindow/SaveToSessionButton");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &MainMenuState::handleButtonPresses);

	window = mGUI->findWidget<MyGUI::Window>("NewGameWindow");
	window->eventWindowButtonPressed = MyGUI::newDelegate(this, &MainMenuState::handleCancelWindow);
	window->setCaption("Select Level");
	window->setMaxSize(window->getWidth(), window->getHeight());
	window->setMinSize(window->getWidth(), window->getHeight());
	window->setPosition(MyGUI::IntPoint((mViewport->getActualWidth()/2 - 512 + 224),208));

	MyGUI::WidgetPtr staticText;
	char* textName = new char[6];
	int totalCollected = 0;
	//open and load the config file
	std::string path = utils::getLostMarblesWriteDir()+"\\"+"Save.cfg";
	ImprovedConfigFile saveFile;
	saveFile.load(path,"=:\t",false);

	//set iterator only over progress values
	Ogre::ConfigFile::SettingsIterator progIter = saveFile.getSettingsIterator("Progress");
	std::string key;
	std::string value;
	while(progIter.hasMoreElements())
	{
		key = progIter.peekNextKey();
		totalCollected += StringConverter::parseInt(progIter.peekNextValue());
		progIter.getNext();
	}

	ConfigFile cf;
	cf.load(path,"=:\t",false);
	std::string sectionProgress = "Progress";
	std::string sectionTotal = "Total Marbles";
	levelNavigation = new MyGUI::WidgetPtr[mLevels.size()];
	std::string s = "";

	for(int i=0; i<mLevels.size(); i++)
	{
		sprintf(textName,"Text%d",i);
		levelNavigation[i] = window->createWidget<MyGUI::StaticText>("StaticText", 10, i*40, window->getWidth(), 40, MyGUI::Align::Center, textName);
		if(totalCollected < (mLevels.at(i).number-1)*3)
		{
			levelNavigation[i]->setEnabled(false);
			levelNavigation[i]->setCaption("Not      Unlocked");
		}
		else
		{
			levelNavigation[i]->setCaption(mLevels.at(i).levelname 
			+ "\t" + cf.getSetting(mLevels.at(i).levelname, sectionProgress,"???") 
			+ " of " +cf.getSetting(mLevels.at(i).levelname, sectionTotal,"???"));
		}
		
		levelNavigation[i]->setFontName("font_Wasser.24");
		levelNavigation[i]->setTextColour(MyGUI::Colour(0,0,0,1));
		levelNavigation[i]->setColour(MyGUI::Colour(0,0,0,0));

		levelNavigation[i]->eventMouseButtonClick = MyGUI::newDelegate(this, &MainMenuState::handleLevelSelected);
		levelNavigation[i]->eventMouseSetFocus = MyGUI::newDelegate(this, &MainMenuState::handleMouseSetFocusOnLevelButton);


		//Ensures first level is always playable, but other than that you need
		//enough collected marbles.

	}

	mLevelFocus = 0;
	setFocusWidget(levelNavigation[mLevelFocus]);
}

void MainMenuState::exit()
{
	if(mGUI)
	{
		mGUI->shutdown();
		delete mGUI;
		mGUI = 0;
	}
	if(mPlatform)
	{
		mPlatform->shutdown();
		delete mPlatform;
		mPlatform = 0;
	}
	Ogre::RenderWindow *win = mRoot->getAutoCreatedWindow();
	if(win)
		win->removeAllViewports();
	if(mSceneMgr)
	{
		mRoot->getSingleton().destroySceneManager(mSceneMgr);
		mSceneMgr = 0;
	}
}

void MainMenuState::createScene()
{
	mSceneMgr->setSkyBox(true,"MenuSkybox");
	mWorld = new OgreBulletDynamics::DynamicsWorld(mSceneMgr, AxisAlignedBox(Vector3(-100,-100,-100), Vector3(100,100,100)), Vector3(0,-29.81,0));
	Entity *ent;
	SceneNode *node;
	OgreBulletCollisions::SphereCollisionShape *shape;
	OgreBulletDynamics::RigidBody *body;

	for(int i = 0; i < 100; i++)
	{
		char entName[16];
		char nodeName[16];
		char bodyName[16];
		sprintf(entName, "marble%d", i);
		sprintf(nodeName, "node%d", i);
		sprintf(bodyName, "body%d", i);
		ent = mSceneMgr->createEntity(entName, "marble.mesh");
		switch(rand() % 5)
		{
		case 0:
			ent->setMaterialName("razvan");			
			break;
		case 1:
			ent->setMaterialName("tyler");
			break;
		case 2:
			ent->setMaterialName("jason");
			break;
		case 3:
			ent->setMaterialName("christie");
			break;
		case 4:
			ent->setMaterialName("conner");
			break;
		}
		node = mSceneMgr->getRootSceneNode()->createChildSceneNode(nodeName);
		node->attachObject(ent);
		node->setScale(Vector3(1.0, 1.0, 1.0));
		shape = new OgreBulletCollisions::SphereCollisionShape(1.0);
		body = new OgreBulletDynamics::RigidBody(bodyName, mWorld);
		body->setShape(node, shape, 0.9f, 0.5f, 1.0f, Vector3(rand()%30 - 15, rand() % 10 + 10, rand() % 30 - 15));
		body->getBulletRigidBody()->setCollisionFlags(body->getBulletRigidBody()->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}

	ent = mSceneMgr->createEntity("hollowSphere", "hollowsphere.mesh");
	OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(ent);
    OgreBulletCollisions::TriangleMeshCollisionShape *sceneTriMeshShape = trimeshConverter->createTrimesh();
    delete trimeshConverter;
	OgreBulletDynamics::RigidBody *sceneRigid = new OgreBulletDynamics::RigidBody("levelRigid", mWorld);
    node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject (ent);
	ent->setVisible(false);
	sceneRigid->setShape(node, sceneTriMeshShape, 0.9f, 0.6f, 1.0f);
	sceneRigid->getBulletRigidBody()->setAngularVelocity(btVector3(1, 1, 0));

	OgreBulletDynamics::PointToPointConstraint *con = new OgreBulletDynamics::PointToPointConstraint(sceneRigid, Vector3(0, 0, 0));
	mWorld->addConstraint(con);
}

void MainMenuState::pause()
{
	MyGUI::LayoutManager::getInstance().unloadLayout(mMainMenuLayout);
	MyGUI::LayoutManager::getInstance().unloadLayout(mSettingsLayout);
	MyGUI::LayoutManager::getInstance().unloadLayout(mLoadFileLayout);
	MyGUI::LayoutManager::getInstance().unloadLayout(mCreditsLayout);
	mRoot->getAutoCreatedWindow()->removeAllViewports();
}

void MainMenuState::resume()
{
	mViewport = mRoot->getAutoCreatedWindow()->addViewport(mCamera);
	mViewport->setBackgroundColour(ColourValue(0.2, 0.2, 0.2));

	mSoundMgr->PlaySound("bgmMM", NULL, &mSoundMgr->mBgmChannel);

	if(mGUI)
	{
		initializeGUI();
		mGUI->injectMouseMove(mMouse->getMouseState().X.abs, mMouse->getMouseState().Y.abs, mMouse->getMouseState().Z.abs);
	}
}


bool MainMenuState::keyPressed(const OIS::KeyEvent& e)
{
	if(mGUI->findWidget<MyGUI::Window>("SettingsWindow")->isVisible() ||
		mGUI->findWidget<MyGUI::Window>("CreditsWindow")->isVisible())
	{
		mGUI->injectKeyPress(MyGUI::KeyCode::Enum(e.key), e.text);
	}
	if (mGUI->findWidget<MyGUI::Window>("NewGameWindow")->isVisible() )
	{
		if( e.key == OIS::KC_RETURN)
		{
			levelNavigation[mLevelFocus]->eventMouseButtonClick(levelNavigation[mLevelFocus]);
		}
		else if ( e.key == OIS::KC_UP || e.key == OIS::KC_W)
		{
			mLevelFocus--;
			if (mLevelFocus < 0)
			{
				mLevelFocus = 0;
			}
			else
			{
				setFocusWidget(levelNavigation[mLevelFocus]);
				lostFocusWidget(levelNavigation[mLevelFocus+1]);
			}
		}
		else if ( e.key == OIS::KC_DOWN || e.key == OIS::KC_S)
		{
			mLevelFocus++;
			if (mLevelFocus >= mLevels.size())
			{
				mLevelFocus = mLevels.size()-1;
			}
			else if (!levelNavigation[mLevelFocus]->isEnabled())
				mLevelFocus--;
			else 
			{
				setFocusWidget(levelNavigation[mLevelFocus]);
				lostFocusWidget(levelNavigation[mLevelFocus-1]);
			}
		}
	}
	else if( e.key == OIS::KC_ESCAPE || e.key == OIS::KC_Q)
	{
		mExitGUIRequested = true;
		return true;
	}
	else if( e.key == OIS::KC_RETURN)
	{
		mWidgetNavigation[mWidgetFocus]->eventMouseButtonClick(mWidgetNavigation[mWidgetFocus]);
	}
	else if ( e.key == OIS::KC_UP || e.key == OIS::KC_W)
	{
		mWidgetFocus--;
		if (mWidgetFocus < 0)
		{
			mWidgetFocus = 0;
		}
		else
		{
			setFocusWidget(mWidgetNavigation[mWidgetFocus]);
			lostFocusWidget(mWidgetNavigation[mWidgetFocus+1]);
		}
	}
	else if ( e.key == OIS::KC_DOWN || e.key == OIS::KC_S)
	{
		mWidgetFocus++;
		if (mWidgetFocus > 3)
		{
			mWidgetFocus = 3;
		}
		else 
		{
			setFocusWidget(mWidgetNavigation[mWidgetFocus]);
			lostFocusWidget(mWidgetNavigation[mWidgetFocus-1]);
		}
	}
	return true;
}

bool MainMenuState::keyReleased(const OIS::KeyEvent& e)
{
	if(mGUI)
		mGUI->injectKeyRelease(MyGUI::KeyCode::Enum(e.key));
	return true;
}

bool MainMenuState::frameStarted(const Ogre::FrameEvent& evt)
{
	mTimeElapsed -= evt.timeSinceLastFrame;
	mCamNode->setPosition(Vector3(40.0f * cos(mTimeElapsed), -50.0f, 40.0f * sin(mTimeElapsed)));
	mCamNode->lookAt(Vector3(0,0,0), Ogre::Node::TS_WORLD);

	mWorld->stepSimulation(evt.timeSinceLastFrame, 10);
	mSoundMgr->FrameStarted(mCamNode, evt.timeSinceLastFrame);
	if(mExitGUIRequested)
	{
		QuitGameState * quitState = QuitGameState::getInstance();
		GameManager::getSingletonPtr()->changeState(quitState);
	}
	return true;
}

bool MainMenuState::frameEnded(const Ogre::FrameEvent& evt)
{
	return true;
}

bool MainMenuState::mouseMoved(const OIS::MouseEvent &e)
{
	if(mGUI)
		mGUI->injectMouseMove(e.state.X.abs, e.state.Y.abs, e.state.Z.abs);
	return true;
}

bool MainMenuState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	if(mGUI)
		mGUI->injectMousePress(e.state.X.abs, e.state.Y.abs, MyGUI::MouseButton::Enum(id));
	return true;
}

bool MainMenuState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	if(mGUI)
		mGUI->injectMouseRelease(e.state.X.abs, e.state.Y.abs, MyGUI::MouseButton::Enum(id));
	return true;
}

void MainMenuState::createSounds()
{
	String bgmMM("ruby_cactus.mp3");
	String hover("plop.wav");
	mSoundMgr->MapSound("bgmMM", mSoundMgr->CreateLoopedStream(bgmMM));
	mSoundMgr->MapSound("hover", mSoundMgr->CreateStream(hover));
}


/* GUI Handlers */

void MainMenuState::handleExitGameButton(MyGUI::WidgetPtr _sender)
{
	mExitGUIRequested = true;
}

void MainMenuState::handleSettingsButton(MyGUI::WidgetPtr _sender)
{
	MyGUI::WindowPtr settingsWindow = mGUI->findWidget<MyGUI::Window>("SettingsWindow");
	settingsWindow->setVisibleSmooth(true);
}

void MainMenuState::handleCreditsButton(MyGUI::WidgetPtr _sender)
{
	MyGUI::WindowPtr settingsWindow = mGUI->findWidget<MyGUI::Window>("CreditsWindow");
	settingsWindow->setVisibleSmooth(true);
}

void MainMenuState::handleLoadButton(MyGUI::WidgetPtr _sender)
{
	MyGUI::WindowPtr settingsWindow = mGUI->findWidget<MyGUI::Window>("NewGameWindow");
	settingsWindow->setVisibleSmooth(true);

	//if (settingsWindow->getChildCount <= 0)
	//{
	//	MyGUI::WidgetPtr staticText;
	//	char* textName = new char[6];
	//	for(int i=0; i<mLevels.size(); i++)
	//	{
	//		sprintf(textName,"Text%d",i);
	//		staticText = mGUI->createWidget<MyGUI::StaticText>("StaticText", 10, i*50, 400, 40, MyGUI::Align::Center, textName);
	//		staticText->setCaption(mLevels.at(i).levelname);
	//		staticText->setColour(MyGUI::Colour(0,0,0,0));
	//		staticText->setFontName("font_Wasser.36");
	//		staticText->setTextColour(MyGUI::Colour(0,0,0,1));
	//		staticText->attachToWidget(window);
	//	}
	//}
}

void MainMenuState::handleMouseSetFocusOnButton(MyGUI::WidgetPtr _sender, MyGUI::WidgetPtr _old)
{
	int i = 0;
	for (i = 0; i < 4 ; i++)
	{
		if (mWidgetNavigation[i] == _sender)
		{
			mWidgetFocus = i;
			setFocusWidget(_sender);
		}
		else
			lostFocusWidget(mWidgetNavigation[i]);
	}
}

void MainMenuState::handleMouseLostFocusOnButton(MyGUI::WidgetPtr _sender, MyGUI::WidgetPtr _new)
{
}

void MainMenuState::handleCancelWindow(MyGUI::WidgetPtr _widget, const std::string& _name)
{
	if (_name == "close") {
		MyGUI::WindowPtr window = MyGUI::castWidget<MyGUI::Window>(_widget);
		window->setVisibleSmooth(false);
	}
}

void MainMenuState::saveSounds()
{
	std::string settingsPath = utils::getLostMarblesWriteDir() + "\\Sound.cfg";
	ImprovedConfigFile soundSettings;
	soundSettings.load(settingsPath,"=:\t", false);
	soundSettings.setSetting("Effects Volume",mEffectVolume);
	soundSettings.setSetting("Music Volume",mMusicVolume);
	soundSettings.setSetting("Mute", mMuted);
	soundSettings.save();
}

void MainMenuState::handleButtonPresses(MyGUI::WidgetPtr _sender)
{
	if(_sender->getName()=="SettingsWindow/SaveToSessionButton")
	{
		MyGUI::WindowPtr window = mGUI->findWidget<MyGUI::Window>("SettingsWindow");
		window->setVisibleSmooth(false);
	}
	else if(_sender->getName()=="SettingsWindow/SaveToFileButton")
	{
		saveSounds();
		mVideoSettings.save();
	}
}

void MainMenuState::handleEffectSlider(MyGUI::WidgetPtr _sender, unsigned int _pos)
{
	MyGUI::HScrollPtr slider = MyGUI::castWidget<MyGUI::HScroll>(_sender);
	mEffectVolume = slider->getScrollPosition();
	FMOD_ChannelGroup_SetVolume(mSoundMgr->mEffectChannelGroup,(float)mEffectVolume/100.0);
}

void MainMenuState::handleMusicSlider(MyGUI::WidgetPtr _sender, unsigned int _pos)
{
	MyGUI::HScrollPtr slider = MyGUI::castWidget<MyGUI::HScroll>(_sender);
	mMusicVolume = slider->getScrollPosition();
	FMOD_ChannelGroup_SetVolume(mSoundMgr->mMusicChannelGroup,(float)mMusicVolume/100.0);
}

void MainMenuState::handleMuteSounds(MyGUI::WidgetPtr _sender)
{
	mMuteCheckBox = _sender->castType<MyGUI::Button>();
	mMuted = !mMuteCheckBox->getStateCheck();
	mMuteCheckBox->setStateCheck(mMuted);
	mSoundMgr->SetMute(mMuted);
}

void MainMenuState::handleFullScreen(MyGUI::WidgetPtr _sender)
{
	MyGUI::ButtonPtr fullScreenCheckBox = _sender->castType<MyGUI::Button>();
	fullScreenCheckBox->setStateCheck(!fullScreenCheckBox->getStateCheck());
	mVideoSettings.setSetting("Full Screen", fullScreenCheckBox->getStateCheck(),"Direct3D9 Rendering Subsystem");
}

void MainMenuState::handleVSync(MyGUI::WidgetPtr _sender)
{
	MyGUI::ButtonPtr vSyncCheckBox = _sender->castType<MyGUI::Button>();
	vSyncCheckBox->setStateCheck(!vSyncCheckBox->getStateCheck());
	mVideoSettings.setSetting("VSync", vSyncCheckBox->getStateCheck(),"Direct3D9 Rendering Subsystem");
}


void MainMenuState::handleResolution(MyGUI::ComboBox* _sender, size_t _index)
{
	Ogre::String key = "Video Mode";
	Ogre::String param;

	switch(_index)
	{
	case 1:
		param = "1024 x 768 @ 32-bit colour";
		break;
	case 0:
		param = "1152 x 864 @ 32-bit colour";
		break;
	case 2:
		param = "1280 x 1024 @ 32-bit colour";
		break;
	}
	mVideoSettings.setSetting(key, param, "Direct3D9 Rendering Subsystem");
}

void MainMenuState::handleAA(MyGUI::ComboBox* _sender, size_t _index)
{
	Ogre::String key = "Anti aliasing";
	Ogre::String param;
	switch(_index)
	{
	case 0:
		param = "Level 0";
		break;
	case 1:
		param = "Level 2";
		break;
	case 2:
		param = "Level 4";
		break;
	case 3:
		param = "Level 8";
		break;
	case 4:
		param = "Level 16";
		break;
	}
	mVideoSettings.setSetting(key, param, "Direct3D9 Rendering Subsystem");
}

void MainMenuState::setFocusWidget(MyGUI::WidgetPtr _sender)
{
	int soundChannel = -1;
	mSoundMgr->PlaySound("hover", NULL, &soundChannel);
	_sender->setTextColour(MyGUI::Colour(1, 1, 1, 1));
}
void MainMenuState::lostFocusWidget(MyGUI::WidgetPtr _sender)
{
	_sender->setTextColour(MyGUI::Colour(0, 0, 0, 0));
}

void MainMenuState::handleMouseSetFocusOnLevelButton(MyGUI::WidgetPtr _sender, MyGUI::WidgetPtr _old)
{
	int i = 0;
	for (i = 0; i < mLevels.size() ; i++)
	{
		if (levelNavigation[i] == _sender)
		{
			mLevelFocus = i;
			setFocusWidget(_sender);
		}
		else
			lostFocusWidget(levelNavigation[i]);
	}
}


void MainMenuState::handleLevelSelected(MyGUI::WidgetPtr _sender)
{
	int levelNum = mLevelFocus;
	PlayState* playState = PlayState::getInstance();
	playState->setLevel(levelNum);
	GameManager::getSingletonPtr()->pushState(playState);
}