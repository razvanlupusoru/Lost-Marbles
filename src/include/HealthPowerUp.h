#ifndef HEALTHPOWERUP_H
#define HEALTHPOWERUP_H

#include "Actor.h"

class HealthPowerUp : public Actor {
public:
    HealthPowerUp(Ogre::Entity *ent, OgreBulletDynamics::RigidBody *body);
    bool onCollision(Actor *otherActor, btManifoldPoint& cp) {
        return otherActor->onCollision(this, cp);
    }
    bool onCollision(CompetitorMarble *otherActor, btManifoldPoint& cp);
    SoundManager *mSoundMgr;
};

#endif
