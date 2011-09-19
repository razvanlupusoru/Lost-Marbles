#include <Ogre.h>
#include "MainMenuState.h"
#include "LogoState.h"

using namespace Ogre;

LogoState LogoState::sLogoState;

LogoState::~LogoState()
{
	exit();
}
void LogoState::enter()
{
	oldTime = 0;
	mRoot = Ogre::Root::getSingletonPtr();
	defineResources();
	
	mRoot->initialise(true, "Lost Marbles");

	TextureManager::getSingleton().setDefaultNumMipmaps(5);
	mSoundMgr = new SoundManager();
	mSoundMgr->Initialize();
	mSoundMgr = SoundManager::getSingletonPtr();		
	createSounds();

	mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "LoadingSMInstance");
	mCamera = mSceneMgr->createCamera("LoadingCamera");
	mViewport = mRoot->getAutoCreatedWindow()->addViewport(mCamera);
	mViewport->setBackgroundColour(ColourValue(0.0, 0.0, 0.0));
	mCamera->setAspectRatio(Real(mViewport->getActualWidth()) / Real(mViewport->getActualHeight()));

	mSoundMgr->PlaySound("bgmLS", NULL, &mSoundMgr->mBgmChannel);
	FMOD_CHANNEL *channel= mSoundMgr->GetSoundChannel(mSoundMgr->mBgmChannel);
	FMOD_Channel_SetVolume(channel,0.25);
		
	InputManager *mInputManager;
	mInputManager = new InputManager(mRoot->getAutoCreatedWindow());
	
	mMouse = InputManager::getSingletonPtr()->getMouse();
	const OIS::MouseState &mouseState = mMouse->getMouseState();
	mouseState.width = mViewport->getActualWidth();
	mouseState.height = mViewport->getActualHeight();

	mKeyboard = InputManager::getSingletonPtr()->getKeyboard();

	mSceneMgr->setAmbientLight(ColourValue(1.0, 1.0, 1.0));

	ResourceGroupManager::getSingleton().initialiseResourceGroup("Loading");

	createOverlays();
	mOurTimer.reset();

	loadResources();

	new TextRenderer();

	mExitGUIRequested = false;
}
void LogoState::exit()
{
	mLoadOverlay->hide();
	Ogre::RenderWindow *win = mRoot->getAutoCreatedWindow();
	if(win){
		win->removeAllViewports();
		OverlayManager *man = OverlayManager::getSingletonPtr();
		for (int i = 0; i < mNumOverlays; i++)
		{
			man->getByName(mNameOverlays[i])->clear();
		}
	}
	OverlayManager *man = OverlayManager::getSingletonPtr();
	if(man)
	{
		/*for (int i = 0; i < mNumOverlays; i++)
		{
			man->getByName(mNameOverlays[i])->clear();
		}*/
		mOverlayGuys->clear();
		mOverlay4Guys->clear();
		mOverlay4G->clear();
		mOverlayGirl->clear();
		mOverlay1Girl->clear();
		mOverlay1G->clear();
		mOverlayGame->clear();
		mOverlay1Game->clear();
		mOverlay1Gsq->clear();
		mOverlaypresents->clear();
		mOverlayplugins->clear();
		mOverlayskip->clear();
	}
}

void LogoState::loadResources(void)
{
	// In Debug mode, only "General" has scripts and no loading is caught.
	// In Release mode, "General" is initialized and 14 of its resources are currently loading.
	#ifdef  _DEBUG
	start(mRoot->getAutoCreatedWindow(), 1, 1, 1.0);
#else
	start(mRoot->getAutoCreatedWindow(), 1, 1, (27.0/(27.0+101.0)));
#endif

	// Turn off rendering of everything except overlays
	mSceneMgr->clearSpecialCaseRenderQueues();
	mSceneMgr->addSpecialCaseRenderQueue(RENDER_QUEUE_OVERLAY);
	mSceneMgr->setSpecialCaseRenderQueueMode(SceneManager::SCRQM_INCLUDE);

	// Initialise the rest of the resource groups, use Background loading if in Release mode
#ifdef  _DEBUG
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	ResourceGroupManager::getSingleton().loadResourceGroup(
		ResourceGroupManager::getSingleton().getWorldResourceGroupName(),
		false, true);
#else 
	ResourceBackgroundQueue::getSingleton().initialiseAllResourceGroups();
	ResourceBackgroundQueue::getSingleton().loadResourceGroup(
		ResourceGroupManager::getSingleton().getWorldResourceGroupName());
#endif	

	// Back to full rendering
	mSceneMgr->clearSpecialCaseRenderQueues();
	mSceneMgr->setSpecialCaseRenderQueueMode(SceneManager::SCRQM_EXCLUDE);

	finish();
}
void LogoState::defineResources(void)
{
	String secName, typeName, archName;
	ConfigFile cf;
	cf.load("resources.cfg");

	ConfigFile::SectionIterator seci = cf.getSectionIterator();
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap *settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
		}
	}
}

bool LogoState::frameStarted(const Ogre::FrameEvent& evt)
{
	int secondsPassed = mOurTimer.getMilliseconds()/1000;
	if(mKeyboard->isKeyDown(OIS::KC_SPACE) || (mOurTimer.getMilliseconds()/1000) >= 24)
	{
		MainMenuState *m = MainMenuState::getInstance();
		GameManager::getSingletonPtr()->changeState(m);
	}
	else 
	{
		updateLogoState();
	}
	return true;
}

void LogoState::updateLogoState()
{
	int secondsPassed = mOurTimer.getMilliseconds()/1000;
	if(secondsPassed < 4)
	{
		if (mCurrentOverlay == -1)
		{
			mOverlayGuys->show();
			mOverlay4Guys->show();
			mCurrentOverlay ++;
		}
	}
	else if(secondsPassed < 8)
	{
		if (mCurrentOverlay == 0)
		{
			oldTime = mOurTimer.getMilliseconds();
			fader1 = new OverlayFader(mOverlayGuys, "Marbles/Guys", mOverlay4Guys, "Marbles/4Guys", 0);
			fader2 = new OverlayFader(mOverlayGirl, "Marbles/Christie", mOverlay1Girl, "Marbles/1Girl", 0);
			fader4 = new OverlayFader(mOverlay4G,"Marbles/4G", 0,0,0);
			fader1->startFadeOut(0.5);
			fader2->startFadeIn(0.5);
			fader4->startFadeIn(0.5);
			mCurrentOverlay++;
		}
		fader1->fade((mOurTimer.getMilliseconds() - oldTime)/1000.000);
		fader2->fade((mOurTimer.getMilliseconds() - oldTime)/1000.000);
		fader4->fade((mOurTimer.getMilliseconds() - oldTime)/1000.000);
		oldTime = mOurTimer.getMilliseconds();
	}
	else if(secondsPassed < 12)
	{
		if (mCurrentOverlay == 1)
		{
			fader1 = new OverlayFader(mOverlayGame, "Marbles/Game", mOverlay1Game, "Marbles/1Game", 0);
			fader3 = new OverlayFader(mOverlay1G,"Marbles/1G",0,0,0);
			fader1->startFadeIn(0.5);
			fader3->startFadeIn(0.5);
			fader2->startFadeOut(0.5);
			mCurrentOverlay++;
		}
		fader1->fade((mOurTimer.getMilliseconds() - oldTime)/1000.000);
		fader2->fade((mOurTimer.getMilliseconds() - oldTime)/1000.000);
		fader3->fade((mOurTimer.getMilliseconds() - oldTime)/1000.000);
		oldTime = mOurTimer.getMilliseconds();
	}
	else if(secondsPassed < 16)
	{
		if (mCurrentOverlay == 2)
		{
			fader2 = new OverlayFader(mOverlay1Gsq, "Marbles/1Gsq", mOverlaypresents, "Marbles/presents", 0);
			fader2->startFadeIn(0.5);
			fader1->startFadeOut(0.5);
			fader3->startFadeOut(0.5);
			mCurrentOverlay++;
		}
		fader1->fade((mOurTimer.getMilliseconds() - oldTime)/1000.000);
		fader2->fade((mOurTimer.getMilliseconds() - oldTime)/1000.000);
		fader3->fade((mOurTimer.getMilliseconds() - oldTime)/1000.000);
		oldTime = mOurTimer.getMilliseconds();
	}
	else if(secondsPassed < 20)
	{
		if (mCurrentOverlay == 3)
		{
			fader1 = new OverlayFader(mOverlayplugins, "Marbles/plugins", 0,0,0);
			fader1->startFadeIn(0.5);
			fader2->startFadeOut(0.5);
			fader4->startFadeOut(0.5);
			mCurrentOverlay++;
		}
		fader1->fade((mOurTimer.getMilliseconds() - oldTime)/1000.000);
		fader2->fade((mOurTimer.getMilliseconds() - oldTime)/1000.000);
		fader4->fade((mOurTimer.getMilliseconds() - oldTime)/1000.000);
		oldTime = mOurTimer.getMilliseconds();
	}
	mWindow->update();
}

void LogoState::createOverlays()
{
	OverlayManager&	omgr = OverlayManager::getSingleton();
	mCurrentOverlay = -1;

	mOverlayGuys = (Overlay*)omgr.getByName("Marbles/Guys");
	OverlayElement* Guys = omgr.getOverlayElement("Marbles/GuysPanel");
	Guys->setPosition(mViewport->getActualWidth()/2 - 430, mViewport->getActualHeight()/2 - 320);

	mOverlay4Guys = (Overlay*)omgr.getByName("Marbles/4Guys");
	OverlayElement* FourGuys = omgr.getOverlayElement("Marbles/4GuysPanel");
	FourGuys->setPosition(mViewport->getActualWidth()/2 - 430, mViewport->getActualHeight()/2 + 145);

	mOverlay4G = (Overlay*)omgr.getByName("Marbles/4G");
	OverlayElement* FourG = omgr.getOverlayElement("Marbles/4GPanel");
	FourG->setPosition(mViewport->getActualWidth()/2 - 430, mViewport->getActualHeight()/2 + 145);

	mOverlayGirl = (Overlay*)omgr.getByName("Marbles/Girl");
	OverlayElement* Girl = omgr.getOverlayElement("Marbles/GirlPanel");
	Girl->setPosition(mViewport->getActualWidth()/2 - 190, mViewport->getActualHeight()/2 - 320);

	mOverlay1Girl = (Overlay*)omgr.getByName("Marbles/1Girl");
	OverlayElement* OneGirl = omgr.getOverlayElement("Marbles/1GirlPanel");
	OneGirl->setPosition(mViewport->getActualWidth()/2 - 190, mViewport->getActualHeight()/2 + 145);

	mOverlay1G = (Overlay*)omgr.getByName("Marbles/1G");
	OverlayElement* OneG = omgr.getOverlayElement("Marbles/1GPanel");
	OneG->setPosition(mViewport->getActualWidth()/2 - 190, mViewport->getActualHeight()/2 + 145);

	mOverlayGame = (Overlay*)omgr.getByName("Marbles/Game");
	OverlayElement* Game = omgr.getOverlayElement("Marbles/GamePanel");
	Game->setPosition(mViewport->getActualWidth()/2 - 450, mViewport->getActualHeight()/2 - 320);

	mOverlay1Game = (Overlay*)omgr.getByName("Marbles/1Game");
	OverlayElement* OneGame = omgr.getOverlayElement("Marbles/1GamePanel");
	OneGame->setPosition(mViewport->getActualWidth()/2 + 20, mViewport->getActualHeight()/2 + 145);

	mOverlay1Gsq = (Overlay*)omgr.getByName("Marbles/1Gsq");
	OverlayElement* OneGsq = omgr.getOverlayElement("Marbles/1GsqPanel");
	OneGsq->setPosition(mViewport->getActualWidth()/2 - 236, mViewport->getActualHeight()/2 + 145);

	mOverlaypresents = (Overlay*)omgr.getByName("Marbles/presents");
	OverlayElement* presents = omgr.getOverlayElement("Marbles/presentsPanel");
	presents->setPosition(mViewport->getActualWidth()/2 + 120, mViewport->getActualHeight()/2 + 190);

	mOverlayplugins = (Overlay*)omgr.getByName("Marbles/plugins");
	OverlayElement* plugins = omgr.getOverlayElement("Marbles/pluginsPanel");
	plugins->setPosition(mViewport->getActualWidth()/2 - 512, mViewport->getActualHeight()/2 - 393);

	mOverlayskip = (Overlay*)omgr.getByName("Marbles/skip");
	OverlayElement* skip = omgr.getOverlayElement("Marbles/skipPanel");
	skip->setPosition(mViewport->getActualWidth()/2 - 186, mViewport->getActualHeight() -75);
}

bool LogoState::frameEnded(const Ogre::FrameEvent& evt)
{	
	return true;
}

void LogoState::pause()
{
	//Loading state can not be paused.
}

void LogoState::resume()
{
	//Loading state can not be paused.
}
bool LogoState::keyPressed(const OIS::KeyEvent& e)
{
	return true;
}

bool LogoState::keyReleased(const OIS::KeyEvent& e)
{
	//Loading state does not receive user input.
	return true;
}

bool LogoState::mouseMoved(const OIS::MouseEvent &e)
{
	//Loading state does not receive user input.
	return true;
}

bool LogoState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	//Loading state does not receive user input.
	return true;
}

bool LogoState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	//Loading state does not receive user input.
	return true;
}

/*
ResourceGroupListener Methods
*/
void LogoState::start(RenderWindow* window,unsigned short numGroupsInit = 1,unsigned short numGroupsLoad = 1, Real initProportion = 0.70f)
{
	mWindow = window;
	mNumGroupsInit = numGroupsInit;
	mNumGroupsLoad = numGroupsLoad;
	mInitProportion = initProportion;
	OverlayManager&	omgr = OverlayManager::getSingleton();

	mLoadOverlay = (Overlay*)omgr.getByName("Core/LoadOverlay");
	if (!mLoadOverlay)
	{
		OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,
			"Cannot find loading overlay", "LoadingBar::start");
	}
	mLoadOverlay->show();

	// Save links to the bar and to the loading text, for updates as we go
	mLoadingBarElement = omgr.getOverlayElement("Core/LoadPanel/Bar/Progress");
	mLoadingCommentElement = omgr.getOverlayElement("Core/LoadPanel/Comment");
	mLoadingDescriptionElement = omgr.getOverlayElement("Core/LoadPanel/Description");

	OverlayElement* barContainer = omgr.getOverlayElement("Core/LoadPanel/Bar");
	mProgressBarMaxSize = barContainer->getWidth();
	mLoadingBarElement->setWidth(0);

	// self is listener
	ResourceGroupManager::getSingleton().addResourceGroupListener(LogoState::getInstance());
}

void LogoState::finish(void)
{
	// hide loading screen
	OverlayManager& omgr = OverlayManager::getSingleton();
	//mLoadingDescriptionElement->setCaption("Hit space bar to skip intro");
	//mLoadingDescriptionElement->setParameter("char_height", "40");
	mLoadingDescriptionElement->hide();
	mOverlayskip->show();

	omgr.getOverlayElement("Core/LoadPanel/Bar")->hide();
	mLoadingBarElement->hide();
	mLoadingCommentElement->hide();

	// Unregister listener
	ResourceGroupManager::getSingleton().removeResourceGroupListener(LogoState::getInstance());
}


void LogoState::resourceGroupScriptingStarted(const String& groupName, size_t scriptCount)
{
	assert(mNumGroupsInit > 0 && "You stated you were not going to init "
		"any groups, but you did! Divide by zero would follow...");
	mProgressBarInc = mProgressBarMaxSize * mInitProportion / (Real)scriptCount;
	mProgressBarInc /= mNumGroupsInit;
	mLoadingDescriptionElement->setCaption("Parsing scripts...");
	mLoadingCommentElement->setCaption(groupName);
	updateLogoState();
}
void LogoState::scriptParseStarted(const String& scriptName, bool& skipThisScript)
{
	mLoadingCommentElement->setCaption(scriptName);
	updateLogoState();	
}
void LogoState::scriptParseEnded(const String& scriptName, bool skipped)
{
	mLoadingBarElement->setWidth(
		mLoadingBarElement->getWidth() + mProgressBarInc);
	updateLogoState();
}
void LogoState::resourceGroupScriptingEnded(const String& groupName)
{
}
void LogoState::resourceGroupLoadStarted(const String& groupName, size_t resourceCount)
{
	assert(mNumGroupsLoad > 0 && "You stated you were not going to load "
		"any groups, but you did! Divide by zero would follow...");
	mProgressBarInc = mProgressBarMaxSize * (1-mInitProportion) /
		(Real)resourceCount;
	mProgressBarInc /= mNumGroupsLoad;
	mLoadingDescriptionElement->setCaption("Loading resource group . . .");
	mLoadingCommentElement->setCaption(groupName);
	updateLogoState();
}
void LogoState::resourceLoadStarted(const ResourcePtr& resource)
{
	mLoadingCommentElement->setCaption(resource->getName());
	mLoadingCommentElement->getCaption();
	updateLogoState();
}
void LogoState::resourceLoadEnded(void)
{
	mLoadingBarElement->setWidth(
		mLoadingBarElement->getWidth() + mProgressBarInc);
	updateLogoState();
}
void LogoState::worldGeometryStageStarted(const String& description)
{
	mLoadingCommentElement->setCaption(description);
	updateLogoState();
}
void LogoState::worldGeometryStageEnded(void)
{
	mLoadingBarElement->setWidth(
		mLoadingBarElement->getWidth() + mProgressBarInc);

	updateLogoState();
}
void LogoState::resourceGroupLoadEnded(const String& groupName)
{
}
void LogoState::createSounds()
{
	String bgmLS("kirlianisles.mp3");
	mSoundMgr->MapSound("bgmLS", mSoundMgr->CreateStream(bgmLS));
}
