#ifndef PLAYERBEHAVIOR_H
#define PLAYERBEHAVIOR_H

#include <OIS/OIS.h>
#include "Behavior.h"

#include "Actor.h"
#include "SoundManager.h"
#include "PlayState.h"

class PlayerBehavior : public IBehavior
{
public:
	PlayerBehavior(Actor *actor, Ogre::SceneNode *camNode, OIS::Keyboard *keyboard, OIS::Mouse *mouse) : mActor(actor), mCamNode(camNode), mKeyboard(keyboard), mMouse(mouse) { mJumpTimer.reset(); }
	bool act(float dt);
	void onCollision(Actor *otherActor);
private:
	Actor *mActor;
	Ogre::SceneNode *mCamNode;
	OIS::Keyboard *mKeyboard;
	OIS::Mouse *mMouse;

	Ogre::Timer mJumpTimer;
	Ogre::Timer mResetTimer;
};

#endif