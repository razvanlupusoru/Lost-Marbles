#ifndef GameState_H
#define GameState_H

#include <Ogre.h>
#include <OIS/OIS.h>
 
#include "GameManager.h"
class GameState
{
public:
	virtual void enter() = 0;
	virtual void exit() = 0;

	virtual void pause() = 0;
	virtual void resume() = 0;

	virtual bool keyPressed(const OIS::KeyEvent& e) = 0;
	virtual bool keyReleased(const OIS::KeyEvent& e) = 0;
	virtual bool mouseMoved(const OIS::MouseEvent &e) = 0;
    virtual bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id) = 0;
    virtual bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id) = 0;

	virtual bool frameStarted(const Ogre::FrameEvent& evt) = 0;
	virtual bool frameEnded(const Ogre::FrameEvent& evt) = 0;

	void changeState(GameState* state) { GameManager::getSingletonPtr()->changeState(state); }
	void pushState(GameState* state) { GameManager::getSingletonPtr()->pushState(state); }
	void popState() { GameManager::getSingletonPtr()->popState(); }
protected:
	GameState() { }
};

#endif