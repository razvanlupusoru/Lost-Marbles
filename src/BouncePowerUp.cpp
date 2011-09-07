#include "BouncePowerUp.h"
#include "BouncePowerUpBehavior.h"
#include "CompetitorMarble.h"

BouncePowerUp::BouncePowerUp(Ogre::Entity *ent, OgreBulletDynamics::RigidBody *body, int duration) : Actor(ent, body), mDuration(duration)
{
	mSoundMgr = SoundManager::getSingletonPtr();
	mTimer.reset();
}

bool BouncePowerUp::onCollision(CompetitorMarble *otherActor, btManifoldPoint& cp)
{
	otherActor->addPowerUp("bounce", new BouncePowerUpBehavior(otherActor, mDuration));

	if(mTimer.getMilliseconds() > 500)
	{
		int soundChannel = -1;
		mSoundMgr->PlaySound("bounce", otherActor->mBodyNode, &soundChannel);
		mTimer.reset();
	}

	return false;
}
