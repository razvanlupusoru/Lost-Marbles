#include <Ogre.h>
#include <fstream>

#include "QuitGameState.h"
#include "SoundManager.h"
#include "TextRenderer.h"

using namespace Ogre;

QuitGameState QuitGameState::sQuitGameState;

void QuitGameState::enter()
{
	mRoot = Root::getSingletonPtr();

	mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "Default SceneManager");

	mCamera = mSceneMgr->createCamera("PlayerCam");
	mCamera->setPosition(Vector3(0.0f, 0.0f, 0.0f));

	mViewport = mRoot->getAutoCreatedWindow()->addViewport(mCamera);
	mViewport->setBackgroundColour(ColourValue(0.0, 0.0, 0.0));

	std::string output = "Thanks for playing! :)";

	TextRenderer::getSingleton().addTextBox("quitText", (Ogre::String)output, 100, 200, 1000, 200, Ogre::ColourValue::White, "32");

    mQuitTimer.reset();
}

void QuitGameState::exit()
{
	mRoot->destroySceneManager(mSceneMgr);
}

void QuitGameState::pause()
{
}

void QuitGameState::resume()
{
}

bool QuitGameState::keyPressed(const OIS::KeyEvent& e)
{
	return true;
}

bool QuitGameState::keyReleased(const OIS::KeyEvent& e)
{
	return true;
}

bool QuitGameState::mouseMoved(const OIS::MouseEvent &e)
{
	return true;
}

bool QuitGameState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return true;
}

bool QuitGameState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	return true;
}

bool QuitGameState::frameStarted(const FrameEvent& evt)
{
    if(mQuitTimer.getMilliseconds()<500)
        return true;
    else
        return false;
}

bool QuitGameState::frameEnded(const FrameEvent& evt)
{
	return true;
}
