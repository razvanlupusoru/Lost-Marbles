#ifndef BOUNCEPOWERUP_H
#define BOUNCEPOWERUP_H

#include "Actor.h"
#include "Behavior.h"

class BouncePowerUp : public Actor {
public:
    BouncePowerUp(Ogre::Entity *ent, OgreBulletDynamics::RigidBody *body, int duration);
    bool onCollision(Actor *otherActor, btManifoldPoint& cp) {
        return otherActor->onCollision(this, cp);
    }
    bool onCollision(CompetitorMarble *otherActor, btManifoldPoint& cp);
protected:
    SoundManager* mSoundMgr;
private:
    Ogre::Timer mTimer;
    int mDuration;
};

#endif
