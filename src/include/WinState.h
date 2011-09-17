#ifndef WINSTATE_H
#define WINSTATE_H

#include <Ogre.h>
#include "GameState.h"

#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"

class WinState : public GameState
{
public:
	WinState() { }
	~WinState();

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

	static WinState* getInstance() { return &sWinState; }
protected:

private:
	void redoParticleSystems();

	Ogre::Root *mRoot;
	Ogre::SceneNode *mCamNode;
	Ogre::SceneManager* mSceneMgr;
	Ogre::Viewport* mViewport;
	OIS::Keyboard* mKeyboard;
	OIS::Mouse *mMouse;
	Ogre::Camera* mCamera;
	SoundManager* mSoundMgr;
	bool mMuteSounds;

	ParticleSystem *mParticles[10];
	SceneNode *mSceneNodes[10];
	Timer mTimers[10];

	int mParticleUpdate;

	MyGUI::Gui * mGUI;
	MyGUI::VectorWidgetPtr mWinningLayout;
	
	static WinState sWinState;
};

#endif