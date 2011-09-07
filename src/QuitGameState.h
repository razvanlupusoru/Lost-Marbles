

#ifndef QUITGAMESTATE_H
#define QUITGAMESTATE_H

#include <Ogre.h>

#include "GameState.h"

class QuitGameState : public GameState
{
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

	static QuitGameState* getInstance() { return &sQuitGameState; }
protected:
	QuitGameState() : mContinue(true) { }

	Ogre::Root *mRoot;
	Ogre::SceneManager* mSceneMgr;
	Ogre::Viewport* mViewport;
	OIS::Keyboard* mKeyboard;
	Ogre::Camera* mCamera;
	SoundManager* mSoundMgr;

	Timer mQuitTimer;
    //TODO: probably don't need these
	bool mGameOver;
	bool mMuteSounds;
	int mBarrelRoll, mOhno, mBgm, mWin;

	bool mContinue;
private:
	static QuitGameState sQuitGameState;
};

#endif
