#ifndef UNDERWATERBEHAVIOR_H
#define UNDERWATERBEHAVIOR_H

#include "PowerUp.h"
#include "Actor.h"

class UnderwaterBehavior : public IPowerUp
{
public:
	UnderwaterBehavior(Actor *actor)  : mActor(actor) {}
	void start();
	bool update(float dt) { return true; }
	void reset() { }
	void finish();
private:
	Actor *mActor;
	btScalar mOriginalFriction;
	btVector3 mOriginalGravity;
protected:
	SoundManager* mSoundMgr;
};

#endif
