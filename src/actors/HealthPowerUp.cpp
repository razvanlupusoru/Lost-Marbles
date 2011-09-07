#include "HealthPowerUp.h"
#include "CompetitorMarble.h"

HealthPowerUp::HealthPowerUp(Ogre::Entity *ent, OgreBulletDynamics::RigidBody *body) : Actor(ent, body)
{
	mSoundMgr = SoundManager::getSingletonPtr();
}

bool HealthPowerUp::onCollision(CompetitorMarble *otherActor, btManifoldPoint& cp)
{
	if(otherActor->mHealth < 100)
	{
		int soundChannel = -1;
		mSoundMgr->PlaySound("heal", otherActor->mBodyNode, &soundChannel);
	}
	otherActor->mHealth = 100;
	return false;
}
