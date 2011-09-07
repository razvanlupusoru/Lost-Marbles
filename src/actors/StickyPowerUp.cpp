#include "StickyPowerUp.h"
#include "StickyPowerUpBehavior.h"
#include "CompetitorMarble.h"

StickyPowerUp::StickyPowerUp(Ogre::Entity *ent, OgreBulletDynamics::RigidBody *body, int duration) : Actor(ent, body), mDuration(duration)
{
	mSoundMgr = SoundManager::getSingletonPtr();
	mTimer.reset();
}

bool StickyPowerUp::onCollision(CompetitorMarble *otherActor, btManifoldPoint& cp)
{
	otherActor->addPowerUp("Sticky", new StickyPowerUpBehavior(otherActor, mDuration));

	if(mTimer.getMilliseconds() > 500)
	{
		int soundChannel = -1;
		mSoundMgr->PlaySound("stick", otherActor->mBodyNode, &soundChannel);
		mTimer.reset();
	}

	return false;
}
