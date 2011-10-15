#ifndef STICKYPOWERUPBEHAVIOR_H
#define STICKYPOWERUPBEHAVIOR_H

#include "Behavior.h"
#include "PowerUp.h"
#include "Actor.h"

class StickyPowerUpBehavior : public IPowerUp {
public:
    StickyPowerUpBehavior(Actor *actor, int duration)  : mActor(actor), mDuration(duration) {}
    void start();
    bool update(float dt);
    void reset() {
        mTimeLeft = mDuration;
    }
    void finish();
private:
    Actor *mActor;
    Ogre::MaterialPtr mOriginalMaterial;
    Ogre::ParticleSystem* mParticleSystem;
    Ogre::Real mOriginalRate;
    btScalar mOriginalMass, mOriginalFriction;
    int mDuration;
protected:
    SoundManager* mSoundMgr;
};

#endif
