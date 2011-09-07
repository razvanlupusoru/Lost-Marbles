#ifndef LOSTBEHAVIOR_H
#define LOSTBEHAVIOR_H

#include "Behavior.h"

#include "Actor.h"
#include "PlayState.h"

class LostBehavior : public IBehavior
{
public:
	LostBehavior(Actor *actor) : mActor(actor) { mJumpTimer.reset(); }

	bool act(float dt);
private:
	Actor *mActor;
	Ogre::Timer mJumpTimer;
};

#endif