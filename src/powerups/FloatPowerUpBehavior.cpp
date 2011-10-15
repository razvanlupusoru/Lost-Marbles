#include "FloatPowerUpBehavior.h"

void FloatPowerUpBehavior::start()
{
    mOriginalGravity = mActor->getBody()->getBulletRigidBody()->getGravity();

    mActor->getBody()->getBulletRigidBody()->setGravity(mOriginalGravity * 0.5f);

    Ogre::SceneManager *mSceneMgr = mActor->getEntity()->_getManager();
    mSoundMgr = SoundManager::getSingletonPtr();
    mParticleSystem = mSceneMgr->createParticleSystem(mActor->getEntity()->getName()+"ParticleFire","Particles/floatyBluey");
    mOriginalRate = mParticleSystem->getEmitter(0)->getEmissionRate();
    Ogre::SceneNode *mParticleFireNode = mActor->getEntity()->getParentSceneNode()->createChildSceneNode(mActor->getEntity()->getName()+"ParticleFire");
    mParticleFireNode->attachObject(mParticleSystem);
    mParticleFireNode->setScale(Vector3(0.01, 0.01, 0.01));

    reset(); // resets time left
}

bool FloatPowerUpBehavior::update(float dt)
{
    float percentTimeLeft = mTimeLeft/mDuration;
    mParticleSystem->getEmitter(0)->setEmissionRate(percentTimeLeft * mOriginalRate);
    mParticleSystem->setEmittedEmitterQuota(300 * percentTimeLeft);

    mTimeLeft -= dt;
    return mTimeLeft < 0;
}

void FloatPowerUpBehavior::finish()
{
    int soundChannel = -1;
    //mSoundMgr->PlaySound("wearoff", mActor->getBody()->getParentSceneNode(), &soundChannel);
    mSoundMgr->PlaySound("wearoff", NULL, &soundChannel);

    Ogre::SceneManager *mSceneMgr = mActor->getEntity()->_getManager();
    mSceneMgr->destroyParticleSystem(mActor->getEntity()->getName()+"ParticleFire");
    mSceneMgr->destroySceneNode(mActor->getEntity()->getName()+"ParticleFire");
    mActor->getBody()->getBulletRigidBody()->setGravity(mOriginalGravity);
}