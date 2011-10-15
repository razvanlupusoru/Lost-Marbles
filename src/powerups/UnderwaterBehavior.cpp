#include "UnderwaterBehavior.h"

void UnderwaterBehavior::start()
{
    mOriginalFriction = mActor->getBody()->getBulletRigidBody()->getFriction();
    mOriginalGravity = mActor->getBody()->getBulletRigidBody()->getGravity();

    mActor->getBody()->getBulletRigidBody()->setGravity(mOriginalGravity * 0.25f);
    mActor->getBody()->getBulletRigidBody()->setFriction(0.05f);

    mTimeLeft = 99;
}

void UnderwaterBehavior::finish()
{
    //int soundChannel = -1;
    //mSoundMgr->PlaySound("wearoff", NULL, &soundChannel);

    mActor->getBody()->getBulletRigidBody()->setGravity(mOriginalGravity);
    mActor->getBody()->getBulletRigidBody()->setFriction(mOriginalFriction);
}