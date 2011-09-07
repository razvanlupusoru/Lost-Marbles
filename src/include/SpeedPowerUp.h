#ifndef SPEEDPOWERUP_H
#define SPEEDPOWERUP_H

#include "Actor.h"
#include "Behavior.h"

class SpeedPowerUp : public Actor
{
public:
	SpeedPowerUp(Ogre::Entity *ent, OgreBulletDynamics::RigidBody *body, int duration);
	bool onCollision(Actor *otherActor, btManifoldPoint& cp) { return otherActor->onCollision(this, cp); }
	bool onCollision(CompetitorMarble *otherActor, btManifoldPoint& cp);
protected:
	SoundManager* mSoundMgr;
private:
	Ogre::Timer mTimer;
	int mDuration;
};

#endif
