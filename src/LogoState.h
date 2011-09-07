#ifndef LogoState_H
#define LogoState_H

#include <Ogre.h>
#include "OgreCEGUIRenderer.h"
#include "GameState.h"
#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"
#include "OverlayFader.h"

class	LogoState : public GameState, ResourceGroupListener
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
	bool switchImage();

	bool debug;

	static LogoState* getInstance() { return &sLogoState; }

protected:
	LogoState() { }
	~LogoState();

	Ogre::Root *mRoot;
	Ogre::SceneManager* mSceneMgr;
	Ogre::Viewport* mViewport;
	OIS::Keyboard* mKeyboard;
	OIS::Mouse *mMouse;
	Ogre::Camera* mCamera;
	SoundManager* mSoundMgr;
	TextRenderer* mTextRenderer;
	Timer mOurTimer;

	RenderWindow* mWindow;
	Overlay* mLoadOverlay;
	Real mInitProportion;
	unsigned short mNumGroupsInit;
	unsigned short mNumGroupsLoad;
	Real mProgressBarMaxSize;
	Real mProgressBarScriptSize;
	Real mProgressBarInc;
	OverlayElement* mLoadingBarElement;
	OverlayElement* mLoadingDescriptionElement;
	OverlayElement* mLoadingCommentElement;

	bool mExitGUIRequested;

private:	
	static LogoState sLogoState;

	void loadResources(void);
	void defineResources(void);
	void createOverlays();
	void createSounds();
	void updateLogoState();
	void initGUI();

	int mNumOverlays;
	int mCurrentOverlay;
	String* mNameOverlays;
	MyGUI::Gui* mGUI;
	MyGUI::VectorWidgetPtr mLogoStateLayout;

	Overlay* mOverlayGuys;
	Overlay* mOverlay4Guys;
	Overlay* mOverlay4G;
	Overlay* mOverlayGirl;
	Overlay* mOverlay1Girl;
	Overlay* mOverlay1G;
	Overlay* mOverlayGame;
	Overlay* mOverlay1Game;
	Overlay* mOverlay1Gsq;
	Overlay* mOverlaypresents;
	Overlay* mOverlayplugins;
	Overlay* mOverlayskip;

	OverlayFader *fader1, *fader2, *fader3, *fader4;
	unsigned long oldTime;

	void start(RenderWindow*, unsigned short, unsigned short, Real);
	void finish(void);
	void resourceGroupScriptingStarted(const String&, size_t);
	void scriptParseStarted(const String&, bool&);
	void scriptParseEnded(const String&, bool);
	void resourceGroupScriptingEnded(const String&);
	void resourceGroupLoadStarted(const String&, size_t);
	void resourceLoadStarted(const ResourcePtr&);
	void resourceLoadEnded(void);
	void worldGeometryStageStarted(const String&);
	void worldGeometryStageEnded(void);
	void resourceGroupLoadEnded(const String&);
};

#endif