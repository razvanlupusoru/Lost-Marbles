#ifndef PLAYSTATE_H
#define PLAYSTATE_H

#include <Ogre.h>
#include <OgreBulletDynamicsWorld.h>
#include <map>

#include "LuaBinding.h"

#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"

#include "GameState.h"
#include "Actor.h"
#include "PauseState.h"
#include "DotLevelLoader.h"

#include "Hydrax.h"
#include "Noise/Perlin/Perlin.h"
#include "Modules/ProjectedGrid/ProjectedGrid.h"

class PlayState : public GameState
{
public:
	static enum Mode{NORMAL, DEBUG, TRANSITION};
	void enter();
	void loadSavedGame(std::string savedGameFilename);
	bool saveGame();
	void resetLevel();
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

	static bool contactAddedWrapper(btManifoldPoint& cp, const btCollisionObject* colObj0, int partId0, int index0, const btCollisionObject* colObj1, int partId1, int index1);
	bool contactAdded(btManifoldPoint& cp, const btCollisionObject* colObj0, int partId0, int index0, const btCollisionObject* colObj1, int partId1, int index1);
	static void internalTickCallbackWrapper(btDynamicsWorld *world, btScalar timeStep);
	void internalTickCallback(btDynamicsWorld *world, btScalar timeStep);

	void loadHelpLayout(const std::string & layoutFile);
	
	Mode getCurrentState(void);
	void setLevel(int);

	static PlayState* getInstance() { return &sPlayState; }
protected:
	PlayState() : mLoadSavedGame(false) { }

	Ogre::Root *mRoot;
	Ogre::SceneManager* mSceneMgr;
	Ogre::Viewport* mViewport;
	OIS::Keyboard* mKeyboard;
	OIS::Mouse* mMouse;
	Ogre::Camera* mCamera;
	Ogre::RenderWindow *mWindow;
	Ogre::SceneNode* mCamNode;
	Ogre::SceneNode* mDebugCamNode;
	Ogre::SceneNode* mTransitionCamNode;
	SoundManager* mSoundMgr;
	lua_State* mLuaState;

	Ogre::Real mRotate;

	CompetitorMarble *mFocusActor;
	Ogre::Real mTransitionRotate;
	float mTransitionRadius;
	float mTransitionHeight;

	bool mContinue;
private:
	void updateScore();
	void updateLife();
	void createScene();
	void clearScene();
	void restoreActors(std::vector<std::string> &listOfNames);
	void loadHydrax();
	void resetIndicator();
	void alterPlayState(Mode);
	void createSounds();
	void pauseGame();

	Hydrax::Hydrax *mHydrax;
	String mHydraxConfig;
	bool mLastFrameUnderwater;

	OgreBulletDynamics::DynamicsWorld *mWorld;
	OgreBulletCollisions::DebugDrawer *mDebugDrawer;
	Mode mCurrentState;
	Mode mDebugShapesOn;
	std::map<Ogre::String, Actor*> mActors;
	std::vector<CompetitorMarble*> mFriends;
	std::vector<CompetitorMarble*> mRivals;
	std::vector<CollectibleMarble*> mTargets;
	std::vector<LevelInformation> mLevels;
	unsigned int mCurrentLevel;
	int mNumTargets;
	int mFriendScore;
	int mFriendLastScore;
	int mRivalScore;
	int mRivalLastScore;

	Ogre::RenderTexture *mRenderTexture;
	
	static PlayState sPlayState;
	bool mLoadSavedGame;
	std::string mSavedGameFilename;

	MyGUI::Gui * mGUI;
	MyGUI::VectorWidgetPtr mTransitionLayout;
	MyGUI::VectorWidgetPtr mConsoleLayout;
	MyGUI::VectorWidgetPtr mPlayStateLayout;
	MyGUI::VectorWidgetPtr mHelpLayout;
	void initGUI();
	void updateGUI();
	void updatePowerups();

	/* Methods available via console */

	/* End methods available via console */

	/* HUD */
	MyGUI::StaticTextPtr mPlayerScoreLabel;
	MyGUI::StaticTextPtr mRivalScoreLabel;
	MyGUI::StaticTextPtr mTotalScoreLabel;
	std::vector<MyGUI::StaticImagePtr> mCollectedImages;
	MyGUI::ProgressPtr mLifeProgressBar;
	MyGUI::StaticImagePtr mStickyImage;
	MyGUI::StaticImagePtr mFloatImage;
	MyGUI::StaticImagePtr mBouncyImage;
	MyGUI::StaticImagePtr mSpeedImage;
	MyGUI::StaticTextPtr mStickyTimer;
	MyGUI::StaticTextPtr mFloatTimer;
	MyGUI::StaticTextPtr mBouncyTimer;
	MyGUI::StaticTextPtr mSpeedTimer;
	/* End HUD */

	/* GUI Handlers */
	void notifyWindowButtonPressed(MyGUI::Window* _sender, const std::string& _button);
	void notifyMouseButtonClick(MyGUI::Widget* _sender);
	void notifyComboAccept(MyGUI::ComboBox* _sender, size_t _index);
	void notifyButtonPressed(MyGUI::Widget* _sender, MyGUI::KeyCode _key, MyGUI::Char _char);
	void addToConsole(const MyGUI::UString & _line);
	int interpretConsoleCommand(const MyGUI::UString & command);
	/* End GUI Handlers */

	/*Console GUI*/
	MyGUI::WindowPtr mConsoleWindow;
	MyGUI::EditPtr mListHistory;
	MyGUI::ComboBoxPtr mComboCommand;
	MyGUI::ButtonPtr mButtonSubmit;
	bool mAutocompleted;
	bool firstConsoleInit;
	/* End Console GUI */
};

#endif
