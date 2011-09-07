#include <Ogre.h>

#include "GameManager.h"
#include "InputManager.h"
#include "GameState.h"
#include "MyGUI.h"

using namespace Ogre;

template<> GameManager* Singleton<GameManager>::ms_Singleton = 0;

GameManager::GameManager()
{
	mRoot = 0;
	mInputManager = 0;
	mSoundManager = 0;
}

GameManager::~GameManager()
{
	// clean up all the states
	while (!mStates.empty())
	{
		mStates.back()->exit();
		mStates.pop_back();
	}

	if (mInputManager)
		delete mInputManager;

	if (SoundManager::getSingletonPtr())
		delete mSoundManager;

	if (mRoot)
		delete mRoot;
}

void GameManager::start(GameState* state)
{
	// store and init the new state
	mStates.push_back(state);
	mStates.back()->enter();

	mLogManager = Ogre::LogManager::getSingletonPtr();
	mLogManager->logMessage("State: Entering " + (String)(typeid(*state).name()));

	mRoot = Root::getSingletonPtr();

	mInputManager = InputManager::getSingletonPtr();
	mInputManager->getKeyboard()->setEventCallback(this);
	mInputManager->getMouse()->setEventCallback(this);

	mSoundManager = SoundManager::getSingletonPtr();
	mTextRenderer = TextRenderer::getSingletonPtr();

	mRoot->addFrameListener(this);

	mRoot->startRendering();
}

void GameManager::changeState(GameState* state)
{
	mSoundManager->StopAllSounds();
	mTextRenderer->removeAllTextBoxes();
	// cleanup the current state
	if ( !mStates.empty() )
	{
		mLogManager->logMessage("State: Exiting " + (String)(typeid(*mStates.back()).name()));
		mStates.back()->exit();
		mStates.pop_back();
	}

	// store and init the new state
	mLogManager->logMessage("State: Entering " + (String)(typeid(*state).name()));
	mStates.push_back(state);
	mStates.back()->enter();
}

void GameManager::pushState(GameState* state)
{
	if((String)(typeid(*state).name()) == "class PauseState"){
		mSoundManager->TogglePause();

		int soundChannel = -1;
		mSoundManager->PlaySound("pause", NULL, &soundChannel);
	}
	else
		mSoundManager->StopAllSounds();

	mTextRenderer->removeAllTextBoxes();
	// pause current state
	if ( !mStates.empty() )
	{
		mLogManager->logMessage("State: Pausing " + (String)(typeid(*mStates.back()).name()));
		mStates.back()->pause();
	}

	// store and init the new state
	mLogManager->logMessage("State: Entering " + (String)(typeid(*state).name()));
	mStates.push_back(state);
	mStates.back()->enter();
}

void GameManager::popState()
{
	if((String)(typeid(*mStates.back()).name()) == "class PauseState"){
		mSoundManager->TogglePause();
	}
	else
		mSoundManager->StopAllSounds();

	mTextRenderer->removeAllTextBoxes();
	// cleanup the current state
	if ( !mStates.empty() )
	{
		mLogManager->logMessage("State: Exiting " + (String)(typeid(*mStates.back()).name()));
		mStates.back()->exit();
		mStates.pop_back();
	}

	// resume previous state
	if ( !mStates.empty() )
	{
		mLogManager->logMessage("State: Resuming " + (String)(typeid(*mStates.back()).name()));
		mStates.back()->resume();
	}
}

void GameManager::setupScene(void)
{
}

bool GameManager::keyPressed(const OIS::KeyEvent& e)
{
	// call keyPressed of current state
	return mStates.back()->keyPressed(e);
}

bool GameManager::keyReleased(const OIS::KeyEvent& e)
{
	// call keyReleased of current state
	return mStates.back()->keyReleased(e);
}

bool GameManager::frameStarted(const FrameEvent& evt)
{
	mInputManager->getKeyboard()->capture();
	mInputManager->getMouse()->capture();
	// call frameStarted of current state
	return mStates.back()->frameStarted(evt);
}

bool GameManager::frameEnded(const FrameEvent& evt)
{
	// call frameEnded of current state
	return mStates.back()->frameEnded(evt);
}

bool GameManager::mouseMoved(const OIS::MouseEvent &e)
{
	return mStates.back()->mouseMoved(e);
}

bool GameManager::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return mStates.back()->mousePressed(e, id);
}

bool GameManager::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return mStates.back()->mouseReleased(e, id);
}

GameManager* GameManager::getSingletonPtr(void)
{
	return ms_Singleton;
}

GameManager& GameManager::getSingleton(void)
{  
	assert(ms_Singleton);
	return *ms_Singleton;
}