#ifndef MAINMENUSTATE_H
#define MAINMENUSTATE_H

#include <Ogre.h>
#include <vector>
#include "GameState.h"
#include "ImprovedConfigFile.h"

#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"

#include "OgreBulletDynamicsWorld.h"

#include "DotLevelLoader.h"

class MainMenuState : public GameState
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

	void setFocusWidget(MyGUI::WidgetPtr _widget);
	void lostFocusWidget(MyGUI::WidgetPtr _widget);

	static MainMenuState* getInstance() { return &sMainMenuState; }
protected:
	MainMenuState() : mExitGUIRequested(false), mGUI(0) { }
	~MainMenuState();

	void createScene();
	void createSounds();

	void handleExitGameButton(MyGUI::WidgetPtr _sender);
	void handleSettingsButton(MyGUI::WidgetPtr _sender);
	void handleCreditsButton(MyGUI::WidgetPtr _sender);
	void handleLoadButton(MyGUI::WidgetPtr _sender);
	void handleMouseSetFocusOnButton(MyGUI::WidgetPtr _sender, MyGUI::WidgetPtr _old);
	void handleMouseLostFocusOnButton(MyGUI::WidgetPtr _sender, MyGUI::WidgetPtr _new);
	void handleCancelWindow(MyGUI::WidgetPtr _widget, const std::string& _name);
	void handleButtonPresses(MyGUI::WidgetPtr _sender);
	void handleEffectSlider(MyGUI::WidgetPtr _sender, unsigned int _pos);
	void handleMusicSlider(MyGUI::WidgetPtr _sender, unsigned int _pos);
	void handleMuteSounds(MyGUI::WidgetPtr _sender);
	void handleVSync(MyGUI::WidgetPtr _sender);
	void handleFullScreen(MyGUI::WidgetPtr _sender);
	void handleResolution(MyGUI::ComboBox* _sender, size_t _index);
	void handleAA(MyGUI::ComboBox* _sender, size_t _index);
	void handleMouseSetFocusOnLevelButton(MyGUI::WidgetPtr _sender, MyGUI::WidgetPtr _old);
	void handleLevelSelected(MyGUI::WidgetPtr _sender);
	//void handleMusicSlider(MyGUI::WidgetPtr _sender);

	Ogre::Root *mRoot;
	Ogre::SceneNode *mCamNode;
	
	Ogre::SceneManager* mSceneMgr;
	Ogre::Viewport* mViewport;
	OIS::Keyboard* mKeyboard;
	OIS::Mouse *mMouse;
	Ogre::Camera* mCamera;
	SoundManager* mSoundMgr;
	OgreBulletDynamics::DynamicsWorld *mWorld;
	bool mMuted;
	size_t mMusicVolume;
	size_t mEffectVolume;
	void saveSounds();
	ImprovedConfigFile mVideoSettings;

	bool mExitGUIRequested;

	std::vector<LevelInformation> mLevels;

	Ogre::Real mTimeElapsed;

private:

	MyGUI::Gui * mGUI;
	MyGUI::OgrePlatform * mPlatform;
	void initializeGUI();
	MyGUI::VectorWidgetPtr mSettingsLayout;
	MyGUI::VectorWidgetPtr mMainMenuLayout;
	MyGUI::VectorWidgetPtr mLoadFileLayout;
	MyGUI::VectorWidgetPtr mCreditsLayout;
	MyGUI::VectorWidgetPtr mNewGameWindowLayout;
	MyGUI::ButtonPtr mMuteCheckBox;

	MyGUI::WidgetPtr* widgetNavigation;
	MyGUI::WidgetPtr* levelNavigation;
	int mWidgetFocus;
	int mLevelFocus;
	
	static MainMenuState sMainMenuState;
};

#endif