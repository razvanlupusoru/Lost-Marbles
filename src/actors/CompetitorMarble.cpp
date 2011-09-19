#include "CompetitorMarble.h"
#include "StaticActor.h"
#include <deque>

CompetitorMarble::CompetitorMarble(Ogre::Entity *ent, OgreBulletDynamics::RigidBody *body) : Actor(ent, body)
{
	mSoundMgr = SoundManager::getSingletonPtr();
	mBody = this->getBody();
	mBodyNode = mEntity->getParentSceneNode();
	mRolling = false;
	mGroundNormal = Vector3::ZERO;
	mRollChannel = -1;
	mScore=0;
	mHealth=100;
	mIsSticky = false;
	mSpeedSum = 0.0;
	mSpeedAvg = 0.0;
	mNeedsHelp = false;
}

bool CompetitorMarble::onCollision(Actor *otherActor, btManifoldPoint &cp)
{
	mGroundNormal = Vector3(cp.m_normalWorldOnB.x(),cp.m_normalWorldOnB.y(),cp.m_normalWorldOnB.z());
	mTimer.reset();
	return otherActor->onCollision(this, cp);
}

bool CompetitorMarble::onCollision(CompetitorMarble *otherActor, btManifoldPoint& cp)
{
	if(otherActor->getEntity()->getName().compare("PlayerMarble")==0)
	{
		if(mScore>0) //if the rival has marbles, player takes one
		{
			otherActor->addCollectedMarble(this->takeAwayOneMarble());
		}
		else if(otherActor->mScore)// if not, we want to take one of his
		{
			this->addCollectedMarble(otherActor->takeAwayOneMarble());
		}
	}
	Ogre::Vector3 competitorVelocity = otherActor->getBody()->getLinearVelocity();
	Ogre::Vector3 velocity = mBody->getLinearVelocity();
	Ogre::Vector3 normal(cp.m_normalWorldOnB.x(), cp.m_normalWorldOnB.y(), cp.m_normalWorldOnB.z());
	Ogre::Real speed = -velocity.dotProduct(normal) + competitorVelocity.dotProduct(normal);
	if(speed > 3.0f)
	{
		int soundChannel = -1;
		mSoundMgr->PlaySound("collide", mBodyNode, &soundChannel);
		FMOD_CHANNEL *channel= mSoundMgr->GetSoundChannel(soundChannel);
		FMOD_Channel_SetVolume(channel,Math::Abs(speed)/50.0);
	}

	return false;
}

bool CompetitorMarble::onCollision(StaticActor *otherActor, btManifoldPoint& cp)
{
	bool behaviorChanged = false, prevSticky = false;
	Ogre::Vector3 competitorVelocity = otherActor->getBody()->getLinearVelocity();
	Ogre::Vector3 velocity = mBody->getLinearVelocity();
	Ogre::Vector3 normal(cp.m_normalWorldOnB.x(), cp.m_normalWorldOnB.y(), cp.m_normalWorldOnB.z());
	Ogre::Real relativeSpeed = -velocity.dotProduct(normal) + competitorVelocity.dotProduct(normal);
	Ogre::Real speed = (velocity - competitorVelocity).normalise();

	if(speeds.size()>15)
	{
		mSpeedSum -= speeds.front();
		speeds.pop_front();
	}
	speeds.push_back(speed);
	mSpeedSum += speed;
	mSpeedAvg = mSpeedSum/16.0;

	if(relativeSpeed > 3.0f && mDropSoundTimer.getMilliseconds() > 100)
	{
		int soundChannel = -1;
		mSoundMgr->PlaySound("drop", mBodyNode, &soundChannel);
		FMOD_CHANNEL *channel= mSoundMgr->GetSoundChannel(soundChannel);
		FMOD_Channel_SetVolume(channel,speed/40.0);
		mDropSoundTimer.reset();
	}

	prevSticky = mIsSticky;
	mIsSticky = mBody->getBulletRigidBody()->getFriction() >= 100.0;
	if(mIsSticky ^ prevSticky)
	{
		behaviorChanged = true;
		mSoundMgr->StopSound(&mRollChannel);
	}

	if((mSpeedAvg > 2.0  && !mRolling) || behaviorChanged)
	{	
		if(mIsSticky)
			mSoundMgr->PlaySound("stickyroll", mBodyNode, &mRollChannel);
		else
			mSoundMgr->PlaySound("roll", mBodyNode, &mRollChannel);
		mRolling = true;
	}
	if(mSpeedAvg <= 2.0)
	{
		mSoundMgr->StopSound(&mRollChannel);
		mRolling = false;
	}
	FMOD_CHANNEL *channel= mSoundMgr->GetSoundChannel(mRollChannel);
	FMOD_Channel_SetVolume(channel,mSpeedAvg/15.0);
	FMOD_Channel_SetFrequency(channel,Math::Sqrt(mSpeedAvg)*13000.0);
	return false;
}

void CompetitorMarble::update(float dt)
{
	if (mBody->getBulletRigidBody()->getWorldTransform().getOrigin().y() < -10)
	{
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(OgreBulletCollisions::OgreBtConverter::to(mSpawnPoint));;
		int soundChannel = -1;
		mSoundMgr->PlaySound("spawn", mBodyNode, &soundChannel);
		mBody->getBulletRigidBody()->clearForces();
		mBody->getBulletRigidBody()->setLinearVelocity(btVector3(0,0,0));
		mBody->getBulletRigidBody()->setAngularVelocity(btVector3(0,0,0));
		mBody->getBulletRigidBody()->setWorldTransform(transform);
		if (mHealth > 0)
		{
			mHealth -= 20;
		}
		removeAllPowerUps();
	}
	if(mTimer.getMilliseconds()>60.0)
	{
		FMOD_CHANNEL *channel= mSoundMgr->GetSoundChannel(mRollChannel);
		FMOD_Channel_SetVolume(channel,0.0);
	}
	if(mTimer.getMilliseconds()>1000.0/60.0)
	{
		mGroundNormal = Vector3::ZERO;
	}
	mFlaggedForRemoval = mHealth <= 0;
	Actor::update(dt);
}

void CompetitorMarble::resetHealth()
{
	mHealth = 100;
}

void CompetitorMarble::addCollectedMarble(const std::string & name)
{
	if(name.compare("")!=0)
	{
		mCollectedMarbles.push_back(name);
		mScore++;
	}
}

std::string CompetitorMarble::takeAwayOneMarble()
{
	if(mCollectedMarbles.size() > 0)
	{
		std::string temp(mCollectedMarbles.back());
		mCollectedMarbles.pop_back();
		mScore--;
		return temp;
	}
	else
		return "";
}

const std::vector<std::string>& CompetitorMarble::getListOfCollectedMarbles() const
{
	return mCollectedMarbles;
}