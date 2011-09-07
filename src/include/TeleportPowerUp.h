#ifndef TELEPORTPOWERUP_H
#define TELEPORTPOWERUP_H

#include "Actor.h"
#include "Behavior.h"

class TeleportPowerUp : public Actor
{
public:
	TeleportPowerUp(Ogre::Entity *ent, OgreBulletDynamics::RigidBody *body, Ogre::String target);
	bool onCollision(Actor *otherActor, btManifoldPoint& cp) { return otherActor->onCollision(this, cp); }
	bool onCollision(CompetitorMarble *otherActor, btManifoldPoint& cp);
protected:
	SoundManager* mSoundMgr;
private:
	Ogre::Timer mTimer;
	Ogre::String mTarget;
	int mDuration;
};

#endif
