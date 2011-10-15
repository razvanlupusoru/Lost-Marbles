#ifndef BLANKSTATE_H
#define BLANKSTATE_H

#include <Ogre.h>

#include "GameState.h"

class BlankState : public GameState {
public:
    void enter();
    void exit();

    void pause();
    void resume();

    bool keyPressed(const OIS::KeyEvent& e);
    bool keyReleased(const OIS::KeyEvent& e);
    bool frameStarted(const Ogre::FrameEvent& evt);
    bool frameEnded(const Ogre::FrameEvent& evt);
    bool mouseMoved(const OIS::MouseEvent &e);
    bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

    static BlankState* getInstance() {
        return &sBlankState;
    }
protected:
    BlankState() : mContinue(true) { }

    Ogre::Root *mRoot;
    Ogre::SceneManager* mSceneMgr;
    Ogre::Viewport* mViewport;
    OIS::Keyboard* mKeyboard;
    Ogre::Camera* mCamera;

    SoundManager* mSoundMgr;
    int mBarrelroll;
    int mSoundChannel;

    bool mContinue;
private:
    static BlankState sBlankState;
};

#endif