#include <Ogre.h>
#include <Shapes/OgreBulletCollisionsSphereShape.h>
#include <Constraints/OgreBulletDynamicsPoint2pointConstraint.h>
#include <Shapes/OgreBulletCollisionsTrimeshShape.h>
#include <Utils/OgreBulletCollisionsMeshToShapeConverter.h>
#include <OgreStringConverter.h>

#include "GameManager.h"
#include "PlayState.h"
#include "WinState.h"
#include "MainMenuState.h"
#include "TextRenderer.h"
#include "QuitGameState.h"

#include "PlayerBehavior.h"
#include "RivalBehavior.h"
#include "UnderwaterBehavior.h"

#include "CompetitorMarble.h"
#include "CollectibleMarble.h"
#include "StaticActor.h"

#include "DotLevelLoader.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"
#include "utils.h"
#include "luahelperfunctions.h"

#include <iostream>
#include <fstream>
#include <map>

using namespace Ogre;

PlayState PlayState::sPlayState;

void PlayState::enter()
{
	mLuaState = lua_open();
	luaL_openlibs(mLuaState);
	luabind::open(mLuaState);
	LuaBinding binder(mLuaState);
	binder.bindLua();

	mKeyboard = InputManager::getSingletonPtr()->getKeyboard();
	mMouse = InputManager::getSingletonPtr()->getMouse();
	mRoot = Root::getSingletonPtr();
	mSoundMgr = SoundManager::getSingletonPtr();
	mGUI = 0;
	mViewport = 0;
	//mCurrentLevel = 0;
	createSounds();
	mContinue = true;
	mRotate = 0.13;
	mFriendScore = 0;
	mFocusActor = 0;
	mRivalScore = 0;
	mFriendLastScore = 0;
	mRivalLastScore = 0;
	firstConsoleInit = true;
	
	Ogre::ShadowTextureManager::getSingletonPtr()->clear();
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_EXTERIOR_CLOSE, "Default SceneManager");
	mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);

	DotLevelLoader loader(mLuaState);
	loader.parseLevels("game.levels", mLevels);
	createScene();

	mViewport = mRoot->getAutoCreatedWindow()->addViewport(mCamera);
	initPlayerCam();
	mViewport->setBackgroundColour(ColourValue(0.0, 0.0, 0.0));

	initGUI();

	mWindow = mRoot->getAutoCreatedWindow();

	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().createManual("RttTex",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, TEX_TYPE_2D, mWindow->getWidth(), mWindow->getHeight(), 0, PF_R8G8B8,
		TU_RENDERTARGET);

	mRenderTexture = texture->getBuffer()->getRenderTarget();
	mRenderTexture->addViewport(mCamera);
	mRenderTexture->getViewport(0)->setBackgroundColour(ColourValue::Black);
	mRenderTexture->getViewport(0)->setOverlaysEnabled(true);
}

void PlayState::setLevel(int levelNum)
{
	mCurrentLevel = levelNum;
}


void PlayState::resetLevel()
{
	clearScene();
	createScene();
	initPlayerCam();
	resetIndicator();
	if(!mLevels[mCurrentLevel].music.empty())
		mSoundMgr->PlaySound(mLevels[mCurrentLevel].music, NULL, &mSoundMgr->mBgmChannel);
}

void PlayState::loadSavedGame(std::string savedGameFilename)
{
	mLoadSavedGame = true;
	mSavedGameFilename = utils::getLostMarblesWriteDir() + "\\" + savedGameFilename;
}

bool PlayState::saveGame()
{
	bool returnFlag = true;
	std::string path = utils::getLostMarblesWriteDir()+"\\"+"Save.cfg";
	ImprovedConfigFile saveFile;
	saveFile.load(path,"=:\t", false);
	std::string test = saveFile.getSetting(mLevels[mCurrentLevel].levelname,"Progress");
	int scoreVal = StringConverter::parseInt(test);
	if(mFriendScore > scoreVal)
	{
		scoreVal=mFriendScore;
	}
	saveFile.setSetting(mLevels[mCurrentLevel].levelname, scoreVal,"Progress");
	saveFile.setSetting(mLevels[mCurrentLevel].levelname, mTargets.size()+mFriendScore,"Total Marbles");
	saveFile.save();

		/*
	std::string savedGameFilenameUpdated = savedGameFilename;
	if(savedGameFilenameUpdated.size()>5)
	{
		if(savedGameFilenameUpdated.substr(savedGameFilenameUpdated.size()-5)!=".lost")
			savedGameFilenameUpdated.append(".lost");
	} 
	else {
		savedGameFilenameUpdated.append(".lost");
	}

	std::string path = utils::getLostMarblesWriteDir()+"\\"+savedGameFilenameUpdated;
	rapidxml::xml_document<> xDoc;
	rapidxml::xml_node<>* xNode = new rapidxml::xml_node<>(rapidxml::node_element);
	rapidxml::xml_attribute<>* xAttribute = new rapidxml::xml_attribute<>();
	std::string temp;

	temp = "currentLevelName";
	xAttribute->name(xDoc.allocate_string(temp.c_str()));
	temp = mLevels[mCurrentLevel].filename;
	xAttribute->value(xDoc.allocate_string(temp.c_str()));
	temp = "LostMarblesSavedGame";
	xNode->name(xDoc.allocate_string(temp.c_str()));
	xNode->append_attribute(xAttribute);
	xDoc.append_node(xNode);

	std::ofstream dotLost;
	dotLost.open(path.c_str());
	dotLost << xDoc;
	if(!dotLost.good())
		returnFlag = false;
	dotLost.close();

	xDoc.clear();
	delete xNode;
	delete xAttribute;
	*/

	return returnFlag;
}

void PlayState::initPlayerCam()
{
	mCamera->setNearClipDistance(1.0);
	mCamera->setFarClipDistance(99999.0);
	mCamera->setProjectionType(PT_PERSPECTIVE);
	mCamera->setAspectRatio(Ogre::Real(mViewport->getActualWidth()) / Ogre::Real(mViewport->getActualHeight()));
}

void PlayState::initGUI()
{
	if(!mGUI)
	{
		MyGUI::OgreRenderManager::getInstance().setSceneManager(mSceneMgr);
		mGUI = MyGUI::Gui::getInstancePtr();
		mGUI->setVisiblePointer(false);
	}
	
	mPlayStateLayout = MyGUI::LayoutManager::getInstancePtr()->loadLayout("PlayState.layout");
	MyGUI::StaticImagePtr image = mGUI->findWidget<MyGUI::StaticImage>("PlayerScoreBG");
	image->setPosition(0,mViewport->getActualHeight()-70);
	image = mGUI->findWidget<MyGUI::StaticImage>("RivalScoreBG");
	image->setPosition(mViewport->getActualWidth()-150,mViewport->getActualHeight()-70);
	if(mRivals.empty())
	{
		image->setVisible(false);
	}
	else
	{
		image->setVisible(true);
	}
	mPlayerScoreLabel = mGUI->findWidget<MyGUI::StaticText>("PlayerScore");
	mPlayerScoreLabel->setPosition(105,mViewport->getActualHeight()-70);
	mRivalScoreLabel = mGUI->findWidget<MyGUI::StaticText>("RivalScore");
	mRivalScoreLabel->setPosition(mViewport->getActualWidth()-125,mViewport->getActualHeight()-70);
	if(mRivals.empty())
	{
		mRivalScoreLabel->setVisible(false);
	}
	else
	{
		mRivalScoreLabel->setVisible(true);
	}
	mTotalScoreLabel = mGUI->findWidget<MyGUI::StaticText>("TotalScore");
	mTotalScoreLabel->setPosition((mViewport->getActualWidth()/2)-10,mViewport->getActualHeight()-70);
	mLifeProgressBar = mGUI->findWidget<MyGUI::Progress>("Health");
	mLifeProgressBar->setPosition(0,50);
	mLifeProgressBar->setProgressRange(100);
	mLifeProgressBar->setProgressPosition(100);
	resetIndicator();
	
	updateGUI();

	mSpeedImage = mGUI->findWidget<MyGUI::StaticImage>("SpeedImage");
	mSpeedTimer = mGUI->findWidget<MyGUI::StaticText>("SpeedTimer");
	mSpeedImage->setPosition(8,mViewport->getActualHeight()-105);
	mSpeedTimer->setPosition(15,mViewport->getActualHeight()-98);
	mSpeedImage->hide();
	mSpeedTimer->hide();
	mFloatImage = mGUI->findWidget<MyGUI::StaticImage>("FloatImage");
	mFloatTimer = mGUI->findWidget<MyGUI::StaticText>("FloatTimer");
	mFloatImage->setPosition(8,mViewport->getActualHeight()-138);
	mFloatTimer->setPosition(15,mViewport->getActualHeight()-131);
	mFloatImage->hide();
	mFloatTimer->hide();
	mBouncyImage = mGUI->findWidget<MyGUI::StaticImage>("BouncyImage");
	mBouncyTimer = mGUI->findWidget<MyGUI::StaticText>("BouncyTimer");
	mBouncyImage->setPosition(8,mViewport->getActualHeight()-171);
	mBouncyTimer->setPosition(15,mViewport->getActualHeight()-164);
	mBouncyImage->hide();
	mBouncyTimer->hide();
	mStickyImage = mGUI->findWidget<MyGUI::StaticImage>("StickyImage");
	mStickyTimer = mGUI->findWidget<MyGUI::StaticText>("StickyTimer");
	mStickyImage->setPosition(8,mViewport->getActualHeight()-204);
	mStickyTimer->setPosition(15,mViewport->getActualHeight()-197);
	mStickyImage->hide();
	mStickyTimer->hide();

	if(firstConsoleInit)
	{
		mConsoleLayout = MyGUI::LayoutManager::getInstancePtr()->loadLayout("Console.layout");
		
		mConsoleWindow = mGUI->findWidget<MyGUI::Window>("ConsoleWindow");
		mConsoleWindow->eventWindowButtonPressed = newDelegate(this, &PlayState::notifyWindowButtonPressed);
		mConsoleWindow->setVisible(false);

		mListHistory = mGUI->findWidget<MyGUI::Edit>("ConsoleWindow_HistoryList");
		mListHistory->setOverflowToTheLeft(true);
		mComboCommand = mGUI->findWidget<MyGUI::ComboBox>("ConsoleWindow_CommandCombo");
		mComboCommand->eventComboAccept = newDelegate(this, &PlayState::notifyComboAccept);
		mComboCommand->eventKeyButtonPressed = newDelegate(this, &PlayState::notifyButtonPressed);
		mButtonSubmit = mGUI->findWidget<MyGUI::Button>("ConsoleWindow_SubmitButton");
		mButtonSubmit->eventMouseButtonClick = newDelegate(this, &PlayState::notifyMouseButtonClick);
		mAutocompleted = false;
		firstConsoleInit = false;
	}
}

void PlayState::updateGUI()
{
	
	Ogre::String text = Ogre::StringConverter::toString(mFriendScore);
	mPlayerScoreLabel->setCaption(text);

	text = Ogre::StringConverter::toString(mRivalScore);
	mRivalScoreLabel->setCaption(text);

	text = Ogre::StringConverter::toString(mNumTargets-mFriendScore-mRivalScore);
	mTotalScoreLabel->setCaption(text);
	
	int i = 0;
	int j = 0;
	while(i<mFriendScore)
	{
		mCollectedImages[i]->setPosition(8+(26*j), mViewport->getActualHeight()-30);
		i++;
		j++;
	}
	j = 0;
	while(i<mFriendScore+mRivalScore)
	{
		mCollectedImages[i]->setPosition((mViewport->getActualWidth()-32)-(26*j),
														mViewport->getActualHeight()-30);
		i++;
		j++;
	}
	j = 0;
	while(i<mNumTargets)
	{
		mCollectedImages[i]->setPosition((mViewport->getActualWidth()/2)-((mNumTargets-mFriendScore-mRivalScore)*26/2)+(26*j),
														mViewport->getActualHeight()-30);
		i++;
		j++;
	}
}

void PlayState::resetIndicator()
{
	for(std::vector<MyGUI::StaticImagePtr>::iterator i = mCollectedImages.begin(); i != mCollectedImages.end(); i++)
	{
		(*i)->deleteAllItems();
	}
	mCollectedImages.clear();

	for(int i=0; i<mNumTargets; i++)
	{
		mCollectedImages.push_back(mGUI->createWidget<MyGUI::StaticImage>("StaticImage", 0, 0, 24, 24, MyGUI::Align::Default, "Main"));
		mCollectedImages.back()->setImageTexture("indicator.png");
		mCollectedImages.back()->setPosition((mViewport->getActualWidth()/2)-(mNumTargets*26/2)+(26*i),
														mViewport->getActualHeight()-30);
	}
}

void PlayState::updatePowerups()
{
	if(mFocusActor)
	{
		std::map<String, IPowerUp*> temp(mFocusActor->getListOfPowerUps());
		if(temp.find("Speed")==temp.end())
		{
			mSpeedImage->hide();
			mSpeedTimer->hide();
		}
		else
		{
			int t = (int)temp["Speed"]->getTimeRemaining();
			String time = Ogre::StringConverter::toString(t) + "\t\tSpeed";
			mSpeedImage->show();
			mSpeedTimer->setCaption(time);
			mSpeedTimer->show();
		}
		if(temp.find("Float")==temp.end())
		{
			mFloatImage->hide();
			mFloatTimer->hide();
		}
		else
		{
			int t = (int)temp["Float"]->getTimeRemaining();
			String time = Ogre::StringConverter::toString(t) + "\t\tFloat";
			mFloatImage->show();
			mFloatTimer->setCaption(time);
			mFloatTimer->show();
		}
		if(temp.find("bounce")==temp.end())
		{
			mBouncyImage->hide();
			mBouncyTimer->hide();
		}
		else
		{
			int t = (int)temp["bounce"]->getTimeRemaining();
			String time = Ogre::StringConverter::toString(t) + "\t\tBouncy";
			mBouncyImage->show();
			mBouncyTimer->setCaption(time);
			mBouncyTimer->show();
		}
		if(temp.find("Sticky")==temp.end())
		{
			mStickyImage->hide();
			mStickyTimer->hide();
		}
		else
		{
			int t = (int)temp["Sticky"]->getTimeRemaining();
			String time = Ogre::StringConverter::toString(t) + "\t\tSticky";
			mStickyImage->show();
			mStickyTimer->setCaption(time);
			mStickyTimer->show();
		}
	}
}

void PlayState::updateScore()
{
	mFriendLastScore = mFriendScore;
	mFriendScore = 0;
	for(std::vector<CompetitorMarble*>::iterator i = mFriends.begin(); i != mFriends.end(); i++)
	{
		mFriendScore += (*i)->mScore;
	}

	mRivalLastScore = mRivalScore;
	mRivalScore = 0;
	for(std::vector<CompetitorMarble*>::iterator i = mRivals.begin(); i != mRivals.end(); i++)
	{
		mRivalScore += (*i)->mScore;
	}

	if((mFriendLastScore != mFriendScore) || (mRivalLastScore != mRivalScore))
		updateGUI();

	if(mFriendScore == mNumTargets)
	{
		saveGame();
		if(++mCurrentLevel < mLevels.size())
		{
			resetLevel();
		}
		else
		{
			GameManager::getSingletonPtr()->changeState(WinState::getInstance());
		}
	}
}

void PlayState::updateLife()
{
	if(mFocusActor)
	{
		mLifeProgressBar->setProgressPosition(mFocusActor->mHealth);
		if(mFocusActor->mHealth >= 50)
			mLifeProgressBar->setColour(MyGUI::Colour("0 1 0.5 1"));
		else if(mFocusActor->mHealth >= 25 && mFocusActor->mHealth < 50)
			mLifeProgressBar->setColour(MyGUI::Colour("1 0.9 0 1"));
		else if(mFocusActor->mHealth < 25)
			mLifeProgressBar->setColour(MyGUI::Colour("1 0.1 0 1"));
	}
}

void PlayState::createScene()
{
	// Needed to separate translate from rotate
	mDebugCamNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("DebugCamNode", Vector3(0.0f, 0.0f, 0.0f));
	mTransitionCamNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("TransitionCamNode", Vector3(0.0f, 0.0f, 0.0f));
	mTransitionRotate=0;
	mTransitionHeight=50;
	mTransitionRadius=50;

	mWorld = new OgreBulletDynamics::DynamicsWorld(mSceneMgr, AxisAlignedBox(Vector3(-1000,-1000,-1000), Vector3(1000,1000,1000)), Vector3(0,-9.801,0));
	mDebugDrawer = new OgreBulletCollisions::DebugDrawer();
	SceneNode *debug = mSceneMgr->getRootSceneNode()->createChildSceneNode("debugDrawer");
	mWorld->setDebugDrawer(mDebugDrawer);
	mDebugShapesOn = NORMAL;

	gContactAddedCallback = &PlayState::contactAddedWrapper;
	mWorld->getBulletDynamicsWorld()->setInternalTickCallback(PlayState::internalTickCallbackWrapper);

	if(mLoadSavedGame)
	{
		rapidxml::file<> savedFile(mSavedGameFilename.c_str());
		rapidxml::xml_document<> XMLDocDotLost;
		XMLDocDotLost.parse<0>( savedFile.data() );
		rapidxml::xml_node<>* pElement;
		pElement = XMLDocDotLost.first_node("LostMarblesSavedGame");
		std::string levelName = "currentLevelName";
		levelName = pElement->first_attribute(levelName.c_str())->value();
		int levelNumber = mCurrentLevel;
		for(std::vector<LevelInformation>::iterator i = mLevels.begin(); i != mLevels.end(); i++)
		{
			if((*i).filename == levelName)
				levelNumber = (*i).number;
		}
		mCurrentLevel = (unsigned int)(levelNumber - 1);
	}

	Ogre::DotLevelLoader loader(mLuaState);
	loader.parseDotLevel(mLevels[mCurrentLevel].filename+".level","General", mSceneMgr);
	loader.parseActors(mLevels[mCurrentLevel].filename+".actors","General", mSceneMgr, mWorld, mActors, mFriends, mRivals, mTargets, &mFocusActor);
	mFocusActor->mNeedsHelp = true;

	mNumTargets = mTargets.size();
	mCamera = mSceneMgr->getCamera("PlayerCam");
	mCamNode = mSceneMgr->getSceneNode("PlayerCam");

	if(mLoadSavedGame)
	{
		mLoadSavedGame = false;
	}

	if(!mLevels[mCurrentLevel].music.empty())
		mSoundMgr->PlaySound(mLevels[mCurrentLevel].music, NULL, &mSoundMgr->mBgmChannel);
	
	if(mViewport)
		mViewport->setCamera(mCamera);
	if(mRenderTexture)
		mRenderTexture->getViewport(0)->setCamera(mCamera);

	PlayState::alterPlayState(PlayState::TRANSITION);
	if(!mGUI)
	{
		MyGUI::OgreRenderManager::getInstance().setSceneManager(mSceneMgr);
		mGUI = MyGUI::Gui::getInstancePtr();
		mGUI->setVisiblePointer(false);
	}
	mTransitionLayout = MyGUI::LayoutManager::getInstance().loadLayout("Transition.layout");
	MyGUI::StaticTextPtr staticText = mGUI->findWidget<MyGUI::StaticText>("LevelNameText");
	staticText->setCaption(mLevels[mCurrentLevel].levelname);
	staticText = mGUI->findWidget<MyGUI::StaticText>("LevelNumberText");
	staticText->setCaption("Level     " + Ogre::StringConverter::toString(mLevels[mCurrentLevel].number));

	try
	{
		MyGUI::StaticImagePtr image = mGUI->findWidget<MyGUI::StaticImage>("RivalScoreBG");
		MyGUI::StaticTextPtr rivalScoreLabel = mGUI->findWidget<MyGUI::StaticText>("RivalScore");

		if(mRivals.empty())
		{
			if(image)
				image->setVisible(false);
			if(rivalScoreLabel)
				rivalScoreLabel->setVisible(false);
		}
		else
		{
			if(image)
				image->setVisible(true);
			if(rivalScoreLabel)
				rivalScoreLabel->setVisible(true);
		}
	}
	catch(MyGUI::Exception &){}
	
}

void PlayState::loadHelpLayout(const std::string & layoutFile)
{
	if(mHelpLayout.empty())
	{
		mHelpLayout = MyGUI::LayoutManager::getInstancePtr()->loadLayout(layoutFile);
	}
}

void PlayState::restoreActors(const std::vector<std::string> &listOfNames)
{
	Ogre::DotLevelLoader loader(mLuaState);
	loader.reloadNodesAndActors(mLevels[mCurrentLevel].filename+".level", mLevels[mCurrentLevel].filename+".actors",listOfNames,"General", mSceneMgr, mWorld, mActors, mFriends, mRivals, mTargets, &mFocusActor);	
}

void PlayState::clearScene()
{
	mTransitionRotate=0;
	mTransitionHeight=50;
	mTransitionRadius=50;

	Ogre::ShadowTextureManager::getSingletonPtr()->clear();
	mSceneMgr->clearScene();
	mSceneMgr->destroyAllCameras();
	
	//mRoot->destroySceneManager(mSceneMgr);
	delete mWorld;
	delete mDebugDrawer;
	while(mActors.size() > 0)
	{
		Actor *a = mActors.begin()->second;
		mActors.erase(mActors.begin());
		delete a;
	}

	mRivals.clear();
	mFriends.clear();
	mTargets.clear();
	mFocusActor = 0;

	SoundManager::getSingletonPtr()->StopAllSounds();
}

void PlayState::exit()
{
	MyGUI::LayoutManager::getInstancePtr()->unloadLayout(mPlayStateLayout);
	mPlayStateLayout.clear();
	for(std::vector<MyGUI::StaticImagePtr>::iterator i = mCollectedImages.begin(); i != mCollectedImages.end(); i++)
	{
		(*i)->deleteAllItems();
	}
	mCollectedImages.clear();

	MyGUI::LayoutManager::getInstancePtr()->unloadLayout(mConsoleLayout);
	mConsoleLayout.clear();

	MyGUI::OgreRenderManager::getInstancePtr()->setSceneManager(NULL);

	alterPlayState(NORMAL);
	mLevels.clear();

	lua_close(mLuaState);
	mLuaState = 0;

	mRoot->getAutoCreatedWindow()->removeAllViewports();

	TextureManager::getSingletonPtr()->remove("RttTex");
	mRenderTexture = 0;
	//TextureManager::getSingletonPtr()->removeAll();
	//MaterialManager::getSingletonPtr()->removeAll();

	clearScene();
	mRoot->destroySceneManager(mSceneMgr);
}

void PlayState::pause()
{
	mConsoleWindow->setVisibleSmooth(false); // Keeping this to keep console history after resuming from pause

	if(!mHelpLayout.empty())
	{
		MyGUI::LayoutManager::getInstancePtr()->unloadLayout(mHelpLayout);
		mHelpLayout.clear();
	}
	MyGUI::LayoutManager::getInstancePtr()->unloadLayout(mPlayStateLayout);
	mPlayStateLayout.clear();
	for(std::vector<MyGUI::StaticImagePtr>::iterator i = mCollectedImages.begin(); i != mCollectedImages.end(); i++)
	{
		(*i)->deleteAllItems();
	}
	mCollectedImages.clear();

	mRoot->getAutoCreatedWindow()->removeAllViewports();
}

void PlayState::resume()
{
	mViewport = mRoot->getAutoCreatedWindow()->addViewport(mCamera);
	initPlayerCam();
	mGUI = 0;
	initGUI();
}

bool PlayState::keyPressed(const OIS::KeyEvent& e)
{

	if(mCurrentState == PlayState::DEBUG || mCurrentState == PlayState::NORMAL)
	{
		if(e.key == OIS::KC_SPACE && !mHelpLayout.empty())
		{
			MyGUI::LayoutManager::getInstancePtr()->unloadLayout(mHelpLayout);
			mHelpLayout.clear();
		}
		else if (e.key == OIS::KC_GRAVE || e.key == OIS::KC_KANJI)
		{
			int soundChannel = -1;
			mSoundMgr->PlaySound("console", NULL, &soundChannel);

			mGUI->setVisiblePointer(!mConsoleWindow->isVisible());
			mConsoleWindow->setVisibleSmooth(!mConsoleWindow->isVisible());
			MyGUI::InputManager::getInstance().setKeyFocusWidget(mComboCommand); 
			mGUI->injectMouseMove(mMouse->getMouseState().X.abs, mMouse->getMouseState().Y.abs, mMouse->getMouseState().Z.abs);
			if(mConsoleWindow->isVisible())
			{
				//set the edit box to accept text input without clicking
				MyGUI::InputManager::getInstance().setKeyFocusWidget(mConsoleWindow->getChildAt(1));
			}
			return true;
		}
		else if(mConsoleWindow->isVisible())
		{
			if(mGUI)
				mGUI->injectKeyPress(MyGUI::KeyCode::Enum(e.key), e.text);
			return true;
		}
		else if (e.key == OIS::KC_SEMICOLON)
		{
			(mCurrentState==DEBUG)? alterPlayState(NORMAL):alterPlayState(DEBUG);
		}
		else if (e.key == OIS::KC_APOSTROPHE || e.key == OIS::KC_COLON)
		{
			(mDebugShapesOn==DEBUG)? mDebugShapesOn = NORMAL : mDebugShapesOn = DEBUG;
			mWorld->setShowDebugShapes(mDebugShapesOn==DEBUG);
			mWorld->setShowDebugContactPoints(mDebugShapesOn==DEBUG);
		}
		else if (e.key == OIS::KC_P || e.key == OIS::KC_ESCAPE)
		{
			pauseGame();
		}
	}
	else if(mCurrentState = PlayState::TRANSITION)
	{
		if (e.key == OIS::KC_SPACE)
		{
			MyGUI::LayoutManager::getInstance().unloadLayout(mTransitionLayout);
			PlayState::alterPlayState(PlayState::NORMAL);
		}
		else if (e.key == OIS::KC_ESCAPE)
		{
			MyGUI::LayoutManager::getInstance().unloadLayout(mTransitionLayout);
			GameManager::getSingletonPtr()->popState();
		}
	}
	return true;
}

bool PlayState::keyReleased(const OIS::KeyEvent& e)
{
	if(mConsoleWindow->isVisible())
	{
		if(mGUI)
			mGUI->injectKeyRelease(MyGUI::KeyCode::Enum(e.key));
	}
	return true;
}

bool PlayState::mouseMoved(const OIS::MouseEvent &e)
{
	if(mGUI && mConsoleWindow->isVisible())
	{
		mGUI->injectMouseMove(e.state.X.abs, e.state.Y.abs, e.state.Z.abs);
		return true;
	}
	switch (mCurrentState) {
		case DEBUG: 
			{
				mDebugCamNode->yaw(Degree(-mRotate * e.state.X.rel), Node::TS_WORLD);
				mDebugCamNode->pitch(Degree(-mRotate * e.state.Y.rel), Node::TS_LOCAL);
			} break;
		case TRANSITION: 
			{
				//mTransitionCamNode->yaw(Degree(-mRotate * e.state.X.rel), Node::TS_WORLD);
				//mTransitionCamNode->pitch(Degree(-mRotate * e.state.Y.rel), Node::TS_LOCAL);
			} break;
		default:
			{
				/*
				Ogre::LogManager* lm = Ogre::LogManager::getSingletonPtr();
				Vector3 oldPos=mCamNode->getPosition();
				Ogre::String x = "oldPos\t" + Ogre::StringConverter::toString(oldPos.x) + "\t"
					+ Ogre::StringConverter::toString(oldPos.y) + "\t"
					+ Ogre::StringConverter::toString(oldPos.x);
				lm->logMessage(x);

				Vector3 temporigin = mFriends.front()->getBody()->getSceneNode()->getPosition();
				Vector3 temptarget = oldPos;
				temptarget.y-=.51;

				btVector3 origin(temporigin.x,temporigin.y,temporigin.z);
				btVector3 target(temptarget.x,temptarget.y,temptarget.z);
				btCollisionWorld::ClosestRayResultCallback result(origin,target);
				mFriends.front()->getBody()->getBulletCollisionWorld()->rayTest(origin,target,result);

				// The boolean operator should not evaluate the second expression if the first one is false
				if(result.hasHit() && result.m_collisionObject->isStaticObject())
				{
					lm->logMessage("hit!");
				}
				else
				{*/
				mCamNode->yaw(Degree(-mRotate * e.state.X.rel), Node::TS_WORLD);
				mCamNode->pitch(Degree(-mRotate * e.state.Y.rel), Node::TS_LOCAL);
					/*lm->logMessage("else!");
				}*/
			} break;
	}
	return true;
}

bool PlayState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	if(mGUI && mGUI->findWidget<MyGUI::Window>("ConsoleWindow")->isVisible())
		mGUI->injectMousePress(e.state.X.abs, e.state.Y.abs, MyGUI::MouseButton::Enum(id));
	return true;
}

bool PlayState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	if(mGUI && mGUI->findWidget<MyGUI::Window>("ConsoleWindow")->isVisible())
		mGUI->injectMouseRelease(e.state.X.abs, e.state.Y.abs, MyGUI::MouseButton::Enum(id));
	return true;
}


bool PlayState::frameStarted(const FrameEvent& evt)
{
	mSoundMgr->FrameStarted(mCamNode, evt.timeSinceLastFrame);
	if (mCurrentState == PlayState::DEBUG)
	{
		// Handles movement of camera during debug mode
		if (mKeyboard->isKeyDown (OIS::KC_W)) 
		{
			mDebugCamNode->translate(mDebugCamNode->getOrientation () * Vector3 (0,0,-30) * evt.timeSinceLastFrame);
		}
		if (mKeyboard->isKeyDown (OIS::KC_S)) 
		{
			mDebugCamNode->translate(mDebugCamNode->getOrientation () * Vector3 (0,0,30) * evt.timeSinceLastFrame);
		}
		if (mKeyboard->isKeyDown (OIS::KC_A)) 
		{
			mDebugCamNode->translate(mDebugCamNode->getOrientation () * Vector3 (-30,0,0) * evt.timeSinceLastFrame);
		}
		if (mKeyboard->isKeyDown (OIS::KC_D)) 
		{
			mDebugCamNode->translate(mDebugCamNode->getOrientation () * Vector3 (30,0,0) * evt.timeSinceLastFrame);
		}
	}
	else if (mCurrentState == PlayState::NORMAL)
	{
		mWorld->stepSimulation(evt.timeSinceLastFrame, 10);

		std::map<String, Actor*>::iterator i = mActors.begin();
		while(i != mActors.end()) {
			Actor* a = i->second;
			a->update(evt.timeSinceLastFrame);
			if(a->removeMe()) {
				mActors.erase(i);
				i = mActors.begin();
				std::vector<CollectibleMarble*>::iterator collectIter = std::find(mTargets.begin(), mTargets.end(), a);
				if(collectIter != mTargets.end())
					mTargets.erase(collectIter);
				std::vector<CompetitorMarble*>::iterator competIter = std::find(mRivals.begin(), mRivals.end(), a);
				if(competIter != mRivals.end())
				{
					this->restoreActors((*competIter)->getListOfCollectedMarbles());
					mRivals.erase(competIter);
				}
				competIter = std::find(mFriends.begin(), mFriends.end(), a);
				if(competIter != mFriends.end())
					mFriends.erase(competIter);
				Ogre::Entity * tempEntity = a->getEntity();
				OgreBulletDynamics::RigidBody * tempBody = a->getBody();
				delete a;
				a = 0;
				mSceneMgr->destroyEntity(tempEntity);
				mWorld->removeObject(tempBody);
				delete tempBody;
			} else ++i;
		}

		if(mFriends.size()==0)
		{
			resetLevel();
		}

		// This is where the particle updates from music are going
		/*
		float * soundData = new float[10];
		mSoundMgr->GetSoundChannel(mSoundMgr->mBgmChannel)->getWaveData(soundData,10,0);
		float average = 0.0f;
		for(int j = 0; j < 10; j++)
		average += soundData[j];
		average /= 10;
		dynamic_cast<Ogre::ParticleSystem*>(mParticleFireNode->getAttachedObject("ParticleFire"))->setDefaultHeight(Ogre::Real(average));
		*/

		updateLife();
		updateScore();
		updatePowerups();

	}
	else if (mCurrentState == PlayState::TRANSITION) // This is temporary so we can move the transition camera manually
	{
		Ogre::Vector3 targetPos = mSceneMgr->getSceneNode("PlayerMarble")->getPosition();

		Ogre::Real xPosition = Ogre::Math::Cos(mTransitionRotate);
		Ogre::Real zPosition = Ogre::Math::Sin(mTransitionRotate);
		mTransitionRotate += evt.timeSinceLastFrame * Ogre::Real(0.5f);

		while(mTransitionRotate > 2 * Ogre::Math::PI)
		{
			mTransitionRotate -= 2 * Ogre::Math::PI;
		}
		mTransitionCamNode->setPosition(
			Vector3(targetPos.x+mTransitionRadius*xPosition,
					mTransitionHeight,
					targetPos.z+mTransitionRadius*zPosition));
		mTransitionRadius-=1*evt.timeSinceLastFrame;
		mTransitionHeight-= (mTransitionCamNode->getPosition().y-targetPos.y)*evt.timeSinceLastFrame/(5*4);
		if(mTransitionRadius<10)
			mTransitionRadius=10;
		mTransitionCamNode->resetOrientation();
		mTransitionCamNode->lookAt(targetPos,Node::TS_WORLD);
		//mTransitionCamNode->resetOrientation();
		//mTransitionCamNode->rotate(Ogre::Vector3::UNIT_Y,Ogre::Radian(-mTransitionRotate*3.14/180),Ogre::Node::TS_WORLD);
	}

	return mContinue;
}

bool PlayState::frameEnded(const FrameEvent& evt)
{
	return true;
}

void PlayState::internalTickCallbackWrapper(btDynamicsWorld *world, btScalar timeStep)
{
	PlayState::getInstance()->internalTickCallback(world, timeStep);
}

void PlayState::internalTickCallback(btDynamicsWorld *world, btScalar timeStep)
{
	for(std::map<String, Actor*>::iterator i = mActors.begin(); i != mActors.end(); i++)
	{
		i->second->internalTick(timeStep);
	}
}

bool PlayState::contactAddedWrapper(btManifoldPoint& cp, const btCollisionObject* colObj0, int partId0, int index0, const btCollisionObject* colObj1, int partId1, int index1)
{
	return PlayState::getInstance()->contactAdded(cp, colObj0, partId0, index0, colObj1, partId1, index1);
}

bool PlayState::contactAdded(btManifoldPoint& cp, const btCollisionObject* colObj0, int partId0, int index0, const btCollisionObject* colObj1, int partId1, int index1)
{
	Actor *a1 = static_cast<Actor*>(colObj0->getUserPointer());
	Actor *a2 = static_cast<Actor*>(colObj1->getUserPointer());
	if(a1 && a2)
	{
		return a1->onCollision(a2, cp) || a2->onCollision(a1, cp);
	}
	return false;
}
void PlayState::alterPlayState(Mode state)
{
	if (mCurrentState != state)
	{
		mCurrentState = state;
		switch (state) {
		case DEBUG:
			{	
				mCamNode->detachObject(mCamera);
				mDebugCamNode->attachObject(mCamera);
				mDebugCamNode->setPosition(0,20,0);
				mDebugCamNode->resetOrientation();
				mDebugCamNode->pitch(Degree(-90), Node::TS_LOCAL);
			} break;
		case TRANSITION:
			{	
				mCamNode->detachObject(mCamera);
				mTransitionCamNode->attachObject(mCamera);
				mTransitionCamNode->setPosition(0,20,0);
				mTransitionCamNode->resetOrientation();
				mTransitionCamNode->pitch(Degree(-90), Node::TS_LOCAL);
			} break;
		case NORMAL:
			{
				mDebugCamNode->detachObject(mCamera);
				mCamNode->attachObject(mCamera);
			} break;
		}
	}
}
PlayState::Mode PlayState::getCurrentState()
{
	return mCurrentState;
}

void PlayState::pauseGame()
{
	mRenderTexture->update();

	PauseState * pauseState = PauseState::getInstance();
	GameManager::getSingletonPtr()->pushState(pauseState);
}

void PlayState::createSounds()
{
	String bgmVoices("Voices.mp3"), bgmCompass("Compass.mp3"), bgmSleep("Sleep.mp3"),
		bgmBlossoms("Blossoms.mp3"), bgmLogic("Logic.mp3"), bgmTrace("Trace.mp3"),
		bgmMarsh("Marsh.mp3"), roll("marbleroll_loop.wav"), jump("jump.wav"),
		drop("marbledrop.wav"), collide("collision.wav"), cry1("cheese3.mp3"),
		cry2("cheese4.mp3"), collect("marblecollect.wav"), spawn("echoplop.wav"),
		console("console.wav"), pause("pause.mp3"), heal("heal.wav"),
		floatsound("boing.wav"), bounce("boing2.wav"), stick("splat.mp3"),
		speed("moonblast.wav"), teleport("teleport.wav"), stickyroll("splatloop.wav"),
		wearoff("wearoff.wav");
	mSoundMgr->MapSound("bgmVoices", mSoundMgr->CreateLoopedStream(bgmVoices));
	mSoundMgr->MapSound("bgmCompass", mSoundMgr->CreateLoopedStream(bgmCompass));
	mSoundMgr->MapSound("bgmSleep", mSoundMgr->CreateLoopedStream(bgmSleep));
	mSoundMgr->MapSound("bgmBlossoms", mSoundMgr->CreateLoopedStream(bgmBlossoms));
	mSoundMgr->MapSound("bgmLogic", mSoundMgr->CreateLoopedStream(bgmLogic));
	mSoundMgr->MapSound("bgmTrace", mSoundMgr->CreateLoopedStream(bgmTrace));
	mSoundMgr->MapSound("bgmMarsh", mSoundMgr->CreateLoopedStream(bgmMarsh));
	mSoundMgr->MapSound("roll", mSoundMgr->CreateLoopedSound(roll));
	mSoundMgr->MapSound("jump", mSoundMgr->CreateSound(jump));
	mSoundMgr->MapSound("drop", mSoundMgr->CreateSound(drop));
	mSoundMgr->MapSound("collide", mSoundMgr->CreateSound(collide));
	mSoundMgr->MapSound("cry1", mSoundMgr->CreateLoopedSound(cry1));
	mSoundMgr->MapSound("cry2", mSoundMgr->CreateLoopedSound(cry2));
	mSoundMgr->MapSound("collect", mSoundMgr->CreateSound(collect));
	mSoundMgr->MapSound("spawn", mSoundMgr->CreateSound(spawn));
	mSoundMgr->MapSound("console", mSoundMgr->CreateStream(console));
	mSoundMgr->MapSound("pause", mSoundMgr->CreateStream(pause));
	mSoundMgr->MapSound("heal", mSoundMgr->CreateSound(heal));
	mSoundMgr->MapSound("float", mSoundMgr->CreateSound(floatsound));
	mSoundMgr->MapSound("bounce", mSoundMgr->CreateSound(bounce));
	mSoundMgr->MapSound("stick", mSoundMgr->CreateSound(stick));
	mSoundMgr->MapSound("speed", mSoundMgr->CreateSound(speed));
	mSoundMgr->MapSound("teleport", mSoundMgr->CreateSound(teleport));
	mSoundMgr->MapSound("stickyroll", mSoundMgr->CreateLoopedSound(stickyroll));
	mSoundMgr->MapSound("wearoff", mSoundMgr->CreateStream(wearoff));
}


/* GUI Handlers */
void PlayState::notifyWindowButtonPressed(MyGUI::Window* _sender, const std::string& _button)
{
	if (_button == "close")
	{
		mGUI->setVisiblePointer(false);
		mConsoleWindow->setVisibleSmooth(false);
	}
}

void PlayState::notifyMouseButtonClick(MyGUI::Widget* _sender)
{
	notifyComboAccept(mComboCommand, MyGUI::ITEM_NONE);
}

void PlayState::notifyComboAccept(MyGUI::ComboBox* _sender, size_t _index)
{
	const MyGUI::UString & command = _sender->getCaption();
	if (command == "")
		return;
	int ret = interpretConsoleCommand(command);
	if (ret == 0)
	{
		addToConsole("Invalid command: " + command);
		_sender->setCaption("");
	}
	else if(ret == 1)
	{
		addToConsole(command);
		_sender->setCaption("");
	}
}

void PlayState::notifyButtonPressed(MyGUI::Widget* _sender, MyGUI::KeyCode _key, MyGUI::Char _char)
{
	size_t len = _sender->getCaption().length();
	MyGUI::Edit* edit = _sender->castType<MyGUI::Edit>();
	if ((_key == MyGUI::KeyCode::Backspace) && (len > 0) && (mAutocompleted))
	{
		edit->deleteTextSelection();
		len = _sender->getCaption().length();
		edit->eraseText(len-1);
	}

	MyGUI::UString command = _sender->getCaption();
	if (command.length() == 0) return;

	mAutocompleted = false;
}

void PlayState::addToConsole(const MyGUI::UString & _line)
{
	if (mListHistory->getCaption().empty())
		mListHistory->addText(_line);
	else
		mListHistory->addText("\n" + _line);

	mListHistory->setTextSelection(mListHistory->getTextLength(), mListHistory->getTextLength());
}

int PlayState::interpretConsoleCommand(const MyGUI::UString & command)
{
	/* This method must be redone in a different way */

	std::string sCommand = (std::string)command;
	Ogre::StringVector splitStrings = Ogre::StringUtil::split(sCommand, " ", 4);
	std::string s = splitStrings.front();
	if(s.compare("close")==0)
	{
		notifyWindowButtonPressed(mConsoleWindow, s);
	}
	else if(s.compare("pause")==0)
	{
		pauseGame();
	}
	else if(s.compare("quit")==0)
	{
		mContinue = false;
	}
	else if(s.compare("push")==0 && splitStrings.size()>=5 )
	{

		if(mActors.find(splitStrings[1]) != mActors.end()){
			OgreBulletDynamics::RigidBody* body = mActors[splitStrings[1]]->getBody();
			if(body)
			{
				Vector3 dir = Vector3(atof(splitStrings[2].c_str()),atof(splitStrings[3].c_str()),atof(splitStrings[4].c_str()));
				body->getBulletObject()->activate();
				body->applyImpulse(dir*50,Vector3(0, 0.05, 0));
			}
			else
			{
				return 0;
			}
		}
	}
	else if(s.compare("move")==0 && splitStrings.size()>=5)
	{
		Actor *actor = mActors[splitStrings[1]];
		if (actor)
		{

			btTransform transform;
			transform.setIdentity();
			transform.setOrigin(OgreBulletCollisions::OgreBtConverter::to(Vector3(atof(splitStrings[2].c_str()),atof(splitStrings[3].c_str()),atof(splitStrings[4].c_str()))));
			actor->getBody()->getBulletRigidBody()->clearForces();
			actor->getBody()->getBulletRigidBody()->setLinearVelocity(btVector3(0,0,0));
			actor->getBody()->getBulletRigidBody()->setAngularVelocity(btVector3(0,0,0));
			actor->getBody()->getBulletRigidBody()->setWorldTransform(transform);
		}
		else
		{
			return 0;
		}
	}
	else if(s.compare("debug")==0)
	{
		if(splitStrings.size()<2)
		{
			alterPlayState(DEBUG);
		} 
		else if(splitStrings[1].compare("off")==0)
		{
			alterPlayState(NORMAL);
		}
	}
	else if(s.compare("win")==0)
	{
		GameManager::getSingletonPtr()->changeState(WinState::getInstance());
		return -1;
	}
	else
	{
		return 0;
	}
	return 1;
}