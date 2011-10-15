#include "StickyPowerUpBehavior.h"

void StickyPowerUpBehavior::start()
{
    //mOriginalMass = 1.0f / mActor->getBody()->getBulletRigidBody()->getInvMass();
    mOriginalFriction = mActor->getBody()->getBulletRigidBody()->getFriction();

    /*btVector3 inertia;
    btVector3 gravity = mActor->getBody()->getBulletRigidBody()->getGravity();
    mActor->getBody()->getBulletRigidBody()->getCollisionShape()->calculateLocalInertia(1.0f, inertia);
    mActor->getBody()->getBulletRigidBody()->setMassProps(1.0f, inertia);
    mActor->getBody()->getBulletRigidBody()->setGravity(gravity);*/
    mActor->getBody()->getBulletRigidBody()->setFriction(100.0f);

    Ogre::SceneManager *sceneMgr = mActor->getEntity()->_getManager();
    mSoundMgr = SoundManager::getSingletonPtr();
    mParticleSystem = sceneMgr->createParticleSystem(mActor->getEntity()->getName()+"FloatyGreeny","Particles/floatyGreeny");
    mOriginalRate = mParticleSystem->getEmitter(0)->getEmissionRate();
    Ogre::SceneNode *particleFireNode = mActor->getEntity()->getParentSceneNode()->createChildSceneNode(mActor->getEntity()->getName()+"FloatyGreeny");
    particleFireNode->attachObject(mParticleSystem);
    particleFireNode->setScale(Vector3(0.001, 0.001, 0.001));

    reset(); // resets time left
}

bool StickyPowerUpBehavior::update(float dt)
{
    float percentTimeLeft = mTimeLeft/mDuration;
    mParticleSystem->getEmitter(0)->setEmissionRate(percentTimeLeft * mOriginalRate);
    mParticleSystem->setEmittedEmitterQuota(300 * percentTimeLeft);

    mTimeLeft -= dt;
    return mTimeLeft < 0;
}

void StickyPowerUpBehavior::finish()
{
    int soundChannel = -1;
    //mSoundMgr->PlaySound("wearoff", mActor->getBody()->getParentSceneNode(), &soundChannel);
    mSoundMgr->PlaySound("wearoff", NULL, &soundChannel);

    Ogre::SceneManager *sceneMgr = mActor->getEntity()->_getManager();
    sceneMgr->destroyParticleSystem(mActor->getEntity()->getName()+"FloatyGreeny");
    sceneMgr->destroySceneNode(mActor->getEntity()->getName()+"FloatyGreeny");

    /*btVector3 inertia;
    btVector3 gravity = mActor->getBody()->getBulletRigidBody()->getGravity();
    mActor->getBody()->getBulletRigidBody()->getCollisionShape()->calculateLocalInertia(mOriginalMass, inertia);
    mActor->getBody()->getBulletRigidBody()->setMassProps(mOriginalMass, inertia);
    mActor->getBody()->getBulletRigidBody()->setGravity(gravity);*/
    mActor->getBody()->getBulletRigidBody()->setFriction(mOriginalFriction);
}