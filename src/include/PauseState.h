#ifndef PAUSESTATE_H
#define PAUSESTATE_H

#include <Ogre.h>
#include "GameState.h"
#include "utils.h"

#include "MyGUI.h"
#include "MyGUI_OgrePlatform.h"

class PauseState : public GameState {
public:
    PauseState() { }
    ~PauseState();

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

    void handleExitGameButton(MyGUI::WidgetPtr _sender);
    void handleResetLevelButton(MyGUI::WidgetPtr _sender);
    void handleResumeGameButton(MyGUI::WidgetPtr _sender);

    void handleMouseSetFocusOnButton(MyGUI::WidgetPtr _sender, MyGUI::WidgetPtr _old);
    void handleMouseLostFocusOnButton(MyGUI::WidgetPtr _sender, MyGUI::WidgetPtr _new);
    void handleCancelWindow(MyGUI::WidgetPtr _widget, const std::string& _name);
    void handleEditAccept(MyGUI::Edit* _sender);

    void handleNotifyListChangePosition(MyGUI::List* _sender, size_t _index);
    void handleSaveButtonPress(MyGUI::WidgetPtr _sender);

    void setFocusWidget(MyGUI::WidgetPtr _widget);
    void lostFocusWidget(MyGUI::WidgetPtr _widget);

    static PauseState* getInstance() {
        return &sPauseState;
    }

private:
    static PauseState sPauseState;

    Ogre::Root *mRoot;
    Ogre::SceneNode *mCamNode;
    Ogre::SceneManager* mSceneMgr;
    Ogre::Viewport* mViewport;
    OIS::Keyboard* mKeyboard;
    OIS::Mouse *mMouse;
    Ogre::Camera* mCamera;
    SoundManager* mSoundMgr;
    bool mMuteSounds;

    MyGUI::WidgetPtr* widgetNavigation;
    int mWidgetFocus;

    Ogre::Rectangle2D *mMiniScreen;

    MyGUI::OgreRenderManager * mRenderManager;
    MyGUI::Gui * mGUI;
    MyGUI::VectorWidgetPtr mSaveGameLayout;
    MyGUI::VectorWidgetPtr mPauseLayout;
    void initializeGUI();
};

#endif