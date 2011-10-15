#ifndef InputManager_H
#define InputManager_H

#include <Ogre.h>
#include <OIS/OIS.h>

class InputManager : public Ogre::Singleton<InputManager> {
public:
    InputManager(Ogre::RenderWindow* rwindow);
    virtual ~InputManager();
    inline OIS::Keyboard* getKeyboard() const {
        return mKeyboard;
    }
    inline OIS::Mouse* getMouse() const {
        return mMouse;
    }
private:
    OIS::InputManager *mInputManager;
    OIS::Keyboard *mKeyboard;
    OIS::Mouse *mMouse;
};

#endif
