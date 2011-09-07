#include "PlayerBehavior.h"
#include "CompetitorMarble.h"

using namespace Ogre;

bool PlayerBehavior::act(float dt)
{
	mCamNode->setPosition(mActor->getBody()->getSceneNode()->getPosition() + mCamNode->getOrientation() * Vector3::UNIT_Z * 6 );
	/*
	Vector3 temporigin = mActor->getBody()->getSceneNode()->getPosition();
	Vector3 temptarget = mCamNode->getPosition();
	temptarget.y-=.45;
	btVector3 origin(temporigin.x,temporigin.y,temporigin.z);
	btVector3 target(temptarget.x,temptarget.y,temptarget.z);
	btCollisionWorld::ClosestRayResultCallback result(origin,target);
	mActor->getBody()->getBulletCollisionWorld()->rayTest(origin,target,result);

	// The boolean operator should not evaluate the second expression if the first one is false
	if(result.hasHit() && result.m_collisionObject->isStaticObject())
	{
		Vector3 newPos = Vector3(result.m_hitPointWorld);
		//Get the normal vector for our intersection
		Vector3 norm = Vector3(result.m_hitNormalWorld);
		norm.normalise();
		//move the new cameras position in the direction of the normal 
		//vector slightly
		//this code might be really weird on a rocky terrain though...
		mCamNode->setPosition(newPos + norm * 0.52);
	}
	*/
	if (PlayState::getInstance()->getCurrentState() == PlayState::NORMAL)
	{
		Vector3 force(0,0,0);
		if(mKeyboard->isKeyDown(OIS::KC_W))
			force += Vector3(0,0,-1);

		if(mKeyboard->isKeyDown(OIS::KC_S))
			force += Vector3(0,0,1);

		if(mKeyboard->isKeyDown(OIS::KC_A))
			force += Vector3(-1,0,0);

		if(mKeyboard->isKeyDown(OIS::KC_D))
			force += Vector3(1,0,0);

		if(mKeyboard->isKeyDown(OIS::KC_SPACE))
		{
			if(!mActor->groundNormal().isZeroLength() && mJumpTimer.getMilliseconds() > 200)
			{
				mJumpTimer.reset();
				mActor->getBody()->getBulletObject()->activate();
				mActor->getBody()->applyImpulse(mActor->groundNormal()*5,Vector3(0, 0, 0));
			}
		}
		if(!mKeyboard->isKeyDown(OIS::KC_J))
		{
			mResetTimer.reset();
		}
		if(mResetTimer.getMilliseconds() > 5000)
		{	
			btTransform transform;
			transform.setIdentity();
			transform.setOrigin(OgreBulletCollisions::OgreBtConverter::to(mActor->getSpawn()));;
			mActor->getBody()->getBulletRigidBody()->clearForces();
			mActor->getBody()->getBulletRigidBody()->setLinearVelocity(btVector3(0,0,0));
			mActor->getBody()->getBulletRigidBody()->setAngularVelocity(btVector3(0,0,0));
			mActor->getBody()->getBulletRigidBody()->setWorldTransform(transform);
			mResetTimer.reset();
		}
		Vector3 apply = mCamNode->getOrientation()*force;
		apply.y = 0;
		apply.normalise();
		if(mKeyboard->isKeyDown(OIS::KC_LSHIFT))
		{
			apply = apply * 0.5f;
		}
		mActor->getBody()->getBulletObject()->activate();
		mActor->getBody()->applyForce(apply * 15.0, Vector3(0, 0.2, 0));
	}
	Vector3 pbPos=mCamNode->getPosition();
	Ogre::LogManager* lm = Ogre::LogManager::getSingletonPtr();
					Ogre::String x = "pbPos   \t" + Ogre::StringConverter::toString(pbPos.x) + "\t"
					+ Ogre::StringConverter::toString(pbPos.y) + "\t"
					+ Ogre::StringConverter::toString(pbPos.x);
				lm->logMessage(x);
	return false;
}
