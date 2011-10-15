#include <Ogre.h>

#include "BlankState.h"
#include "SoundManager.h"

using namespace Ogre;

BlankState BlankState::sBlankState;
/*
SoundManager * soundMgr;
int barrelroll; //temporary stuff. just trying things out.
int bgm;
int musicChannel = -1;
*/

void BlankState::enter()
{
    mKeyboard = InputManager::getSingletonPtr()->getKeyboard();
    mRoot = Root::getSingletonPtr();

    mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "Default SceneManager");

    mCamera = mSceneMgr->createCamera("PlayerCam");
    mCamera->setPosition(Vector3(0.0f, 0.0f, 0.0f));
    /*
    cameraNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("CameraNode", Vector3(0.0f, 0.0f, 0.0f));
    cameraNode->attachObject(mCamera);

    soundMgr = new SoundManager;
    soundMgr->Initialize();
    barrelroll = soundMgr->CreateSound(String("Barrel Roll.mp3"));
    bgm = soundMgr->CreateLoopedSound(String("8bp065-02-anamanaguchi_helix_nebula.mp3"));
    FMOD::Channel *channel= soundMgr->GetSoundChannel(musicChannel);
    channel->setVolume(0.5);
    soundMgr->PlaySound(bgm, cameraNode, &musicChannel);
    */

    mViewport = mRoot->getAutoCreatedWindow()->addViewport(mCamera);
    mViewport->setBackgroundColour(ColourValue(0.0, 1.0, 0.0));

}

void BlankState::exit()
{
    mRoot->destroySceneManager(mSceneMgr);
}

void BlankState::pause()
{
}

void BlankState::resume()
{
}

bool BlankState::keyPressed(const OIS::KeyEvent& e)
{
    if (e.key == OIS::KC_Q || e.key == OIS::KC_ESCAPE) {
        mContinue = false;
    }
    return true;
}

bool BlankState::keyReleased(const OIS::KeyEvent& e)
{
    return true;
}

bool BlankState::mouseMoved(const OIS::MouseEvent &e)
{
    return true;
}

bool BlankState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    if(id == OIS::MB_Left) {
        int soundChannel = -1;
        //soundMgr->PlaySound(barrelroll, cameraNode, &soundChannel);
        mViewport->setBackgroundColour(ColourValue(1,1,0));
    }
    if(id == OIS::MB_Right) {
        int soundChannel = -1;
        //soundMgr->PlaySound(barrelroll, cameraNode, &soundChannel);
        mViewport->setBackgroundColour(ColourValue(0,1,1));
    }
    return true;
}

bool BlankState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    return true;
}

bool BlankState::frameStarted(const FrameEvent& evt)
{
    if(mKeyboard->isKeyDown(OIS::KC_R)) {
        mViewport->setBackgroundColour(ColourValue(1,0,0));
    }
    if(mKeyboard->isKeyDown(OIS::KC_G)) {
        mViewport->setBackgroundColour(ColourValue(0,1,0));
    }
    if(mKeyboard->isKeyDown(OIS::KC_B)) {
        mViewport->setBackgroundColour(ColourValue(0,0,1));
    }
    return mContinue;
}

bool BlankState::frameEnded(const FrameEvent& evt)
{
    return true;
}