#include "SpeedPowerUp.h"
#include "SpeedPowerUpBehavior.h"
#include "CompetitorMarble.h"

SpeedPowerUp::SpeedPowerUp(Ogre::Entity *ent, OgreBulletDynamics::RigidBody *body, int duration) : Actor(ent, body), mDuration(duration)
{
    mSoundMgr = SoundManager::getSingletonPtr();
    mTimer.reset();
}

bool SpeedPowerUp::onCollision(CompetitorMarble *otherActor, btManifoldPoint& cp)
{
    otherActor->addPowerUp("Speed", new SpeedPowerUpBehavior(otherActor, mDuration));

    if(mTimer.getMilliseconds() > 500) {
        int soundChannel = -1;
        mSoundMgr->PlaySound("speed", otherActor->mBodyNode, &soundChannel);
        mTimer.reset();
    }

    return false;
}
