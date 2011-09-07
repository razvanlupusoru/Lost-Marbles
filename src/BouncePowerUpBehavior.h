#ifndef BouncePOWERUPBEHAVIOR_H
#define BOUNCEPOWERUPBEHAVIOR_H

#include "Behavior.h"
#include "PowerUp.h"
#include "Actor.h"

class BouncePowerUpBehavior : public IPowerUp
{
public:
	BouncePowerUpBehavior(Actor *actor, int duration)  : mActor(actor), mDuration(duration) {}
	void start();
	bool update(float dt);
	void reset() { mTimeLeft = mDuration; }
	void finish();
private:
	Actor *mActor;
	Ogre::ParticleSystem* mParticleSystem;
	Ogre::Real mOriginalRate;
	Ogre::Real mOriginalRestitution;
	int mDuration;
protected:
	SoundManager* mSoundMgr;
};

#endif
