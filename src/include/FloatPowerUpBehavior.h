#ifndef FLOATPOWERUPBEHAVIOR_H
#define FLOATPOWERUPBEHAVIOR_H

#include "Behavior.h"
#include "PowerUp.h"
#include "Actor.h"

class FloatPowerUpBehavior : public IPowerUp {
public:
    FloatPowerUpBehavior(Actor *actor, int duration)  : mActor(actor), mDuration(duration) {}
    void start();
    bool update(float dt);
    void reset() {
        mTimeLeft = mDuration;
    }
    void finish();
protected:
    SoundManager* mSoundMgr;
private:
    Actor *mActor;
    Ogre::ParticleSystem* mParticleSystem;
    Ogre::Real mOriginalRate;
    btVector3 mOriginalGravity;
    int mDuration;
};

#endif
