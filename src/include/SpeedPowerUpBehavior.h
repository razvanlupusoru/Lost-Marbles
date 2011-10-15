#ifndef SPEEDPOWERUPBEHAVIOR_H
#define SPEEDPOWERUPBEHAVIOR_H

#include "Behavior.h"
#include "PowerUp.h"
#include "Actor.h"

class SpeedPowerUpBehavior : public IPowerUp {
public:
    SpeedPowerUpBehavior(Actor *actor, int duration)  : mActor(actor), mDuration(duration) {}
    void start();
    bool update(float dt);
    void reset() {
        mTimeLeft = mDuration;
    }
    void finish();
private:
    Actor *mActor;
    Ogre::ParticleSystem* mParticleSystem;
    Ogre::Real mOriginalRate;
    Ogre::Real mOriginalSpeed;
    int mDuration;
protected:
    SoundManager* mSoundMgr;
};

#endif
