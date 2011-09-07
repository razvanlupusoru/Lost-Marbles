#include "SpeedPowerUpBehavior.h"

void SpeedPowerUpBehavior::start()
{		
	Ogre::SceneManager *mSceneMgr = mActor->getEntity()->_getManager();
	mSoundMgr = SoundManager::getSingletonPtr();
	mParticleSystem = mSceneMgr->createParticleSystem(mActor->getEntity()->getName()+"ParticleRed","Particles/floatyRedy");
	mOriginalRate = mParticleSystem->getEmitter(0)->getEmissionRate();

	mOriginalSpeed = mActor->getMaxSpeed();
	mActor->setMaxSpeed(mOriginalSpeed * 2);

	Ogre::SceneNode *mParticleFireNode = mActor->getEntity()->getParentSceneNode()->createChildSceneNode(mActor->getEntity()->getName()+"ParticleRed");
	mParticleFireNode->attachObject(mParticleSystem);
	mParticleFireNode->setScale(Vector3(0.01, 0.01, 0.01));

	reset(); // resets time left
}

bool SpeedPowerUpBehavior::update(float dt)
{
	float percentTimeLeft = mTimeLeft/mDuration;
	mParticleSystem->getEmitter(0)->setEmissionRate(percentTimeLeft * mOriginalRate);
	mParticleSystem->setEmittedEmitterQuota(300 * percentTimeLeft);

	mTimeLeft -= dt;
	return mTimeLeft < 0;
}

void SpeedPowerUpBehavior::finish()
{
	int soundChannel = -1;
	//mSoundMgr->PlaySound("wearoff", mActor->getBody()->getParentSceneNode(), &soundChannel);
	mSoundMgr->PlaySound("wearoff", NULL, &soundChannel);

	Ogre::SceneManager *mSceneMgr = mActor->getEntity()->_getManager();
	mSceneMgr->destroyParticleSystem(mActor->getEntity()->getName()+"ParticleRed");
	mSceneMgr->destroySceneNode(mActor->getEntity()->getName()+"ParticleRed");
	mActor->setMaxSpeed(mOriginalSpeed);
}