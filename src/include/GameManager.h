#ifndef GameManager_H
#define GameManager_H

#include <vector>

#include <Ogre.h>

#include "InputManager.h"
#include "SoundManager.h"
#include "TextRenderer.h"

class GameState;

class GameManager : public Ogre::FrameListener, public OIS::KeyListener, public OIS::MouseListener,	public Ogre::Singleton<GameManager> {
public:
    GameManager();
    ~GameManager();
    void start(GameState* state);
    void changeState(GameState* state);
    void pushState(GameState* state);
    void popState();
    static GameManager& getSingleton(void);
    static GameManager* getSingletonPtr(void);
protected:
    Ogre::Root *mRoot;
    InputManager* mInputManager;
    SoundManager* mSoundManager;
    TextRenderer* mTextRenderer;
    LogManager* mLogManager;

    void setupScene(void);

    bool keyPressed(const OIS::KeyEvent& e);
    bool keyReleased(const OIS::KeyEvent& e);
    bool mouseMoved(const OIS::MouseEvent &e);
    bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

    bool frameStarted(const Ogre::FrameEvent& evt);
    bool frameEnded(const Ogre::FrameEvent& evt);
private:
    std::vector<GameState*> mStates;
};

#endif