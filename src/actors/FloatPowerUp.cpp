#include "FloatPowerUp.h"
#include "FloatPowerUpBehavior.h"
#include "CompetitorMarble.h"

FloatPowerUp::FloatPowerUp(Ogre::Entity *ent, OgreBulletDynamics::RigidBody *body, int duration) : Actor(ent, body), mDuration(duration)
{
    mSoundMgr = SoundManager::getSingletonPtr();
    mTimer.reset();
}

bool FloatPowerUp::onCollision(CompetitorMarble *otherActor, btManifoldPoint& cp)
{
    otherActor->addPowerUp("Float", new FloatPowerUpBehavior(otherActor, mDuration));

    if(mTimer.getMilliseconds() > 500) {
        int soundChannel = -1;
        mSoundMgr->PlaySound("float", otherActor->mBodyNode, &soundChannel);
        mTimer.reset();
    }

    return false;
}
