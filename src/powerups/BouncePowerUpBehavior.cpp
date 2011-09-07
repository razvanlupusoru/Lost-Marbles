#include "BouncePowerUpBehavior.h"

void BouncePowerUpBehavior::start()
{		
	Ogre::SceneManager *mSceneMgr = mActor->getEntity()->_getManager();
	mSoundMgr = SoundManager::getSingletonPtr();
	mParticleSystem = mSceneMgr->createParticleSystem(mActor->getEntity()->getName()+"ParticlePurple","Particles/floatyPurpley");
	mOriginalRate = mParticleSystem->getEmitter(0)->getEmissionRate();

	mOriginalRestitution = mActor->getBody()->getBulletRigidBody()->getRestitution();
	mActor->getBody()->getBulletRigidBody()->setRestitution(0.8f);

	Ogre::SceneNode *mParticleFireNode = mActor->getEntity()->getParentSceneNode()->createChildSceneNode(mActor->getEntity()->getName()+"ParticlePurple");
	mParticleFireNode->attachObject(mParticleSystem);
	mParticleFireNode->setScale(Vector3(0.01, 0.01, 0.01));

	reset(); // resets time left
}

bool BouncePowerUpBehavior::update(float dt)
{
	float percentTimeLeft = mTimeLeft/mDuration;
	mParticleSystem->getEmitter(0)->setEmissionRate(percentTimeLeft * mOriginalRate);
	mParticleSystem->setEmittedEmitterQuota(300 * percentTimeLeft);

	mTimeLeft -= dt;
	return mTimeLeft < 0;
}

void BouncePowerUpBehavior::finish()
{
	int soundChannel = -1;
	//mSoundMgr->PlaySound("wearoff", mActor->getBody()->getParentSceneNode(), &soundChannel);
	mSoundMgr->PlaySound("wearoff", NULL, &soundChannel);

	Ogre::SceneManager *mSceneMgr = mActor->getEntity()->_getManager();
	mSceneMgr->destroyParticleSystem(mActor->getEntity()->getName()+"ParticlePurple");
	mSceneMgr->destroySceneNode(mActor->getEntity()->getName()+"ParticlePurple");
	mActor->getBody()->getBulletRigidBody()->setRestitution(mOriginalRestitution);
}