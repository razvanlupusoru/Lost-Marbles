#include "CollectibleMarble.h"
#include "CompetitorMarble.h"
#include "PlayState.h"

CollectibleMarble::CollectibleMarble(Ogre::Entity *ent, OgreBulletDynamics::RigidBody *body) : Actor(ent, body)
{
    mCryChannel = -1;
    mSoundMgr = SoundManager::getSingletonPtr();
    mBodyNode = mEntity->getParentSceneNode();
    if((rand()%2)>0)
        mSoundMgr->PlaySound("cry1", mBodyNode, &mCryChannel);
    else
        mSoundMgr->PlaySound("cry2", mBodyNode, &mCryChannel);

}

bool CollectibleMarble::onCollision(Actor *otherActor, btManifoldPoint &cp)
{
    mGroundNormal = Vector3(cp.m_normalWorldOnB.x(),cp.m_normalWorldOnB.y(),cp.m_normalWorldOnB.z());
    mTimer.reset();
    return otherActor->onCollision(this, cp);
}

bool CollectibleMarble::onCollision(CompetitorMarble *otherActor, btManifoldPoint& cp)
{
    if(!mFlaggedForRemoval) {
        otherActor->addCollectedMarble(this->getBody()->getName());
        mFlaggedForRemoval = true;
        mSoundMgr->StopSound(&mCryChannel);
        int soundChannel = -1;
        mSoundMgr->PlaySound("collect", mBodyNode, &soundChannel);
    }
    return false;
}

void CollectibleMarble::update(float dt)
{
    if ((mEntity->getParentSceneNode()->getPosition() - mSpawnPoint).length() > 10.0f) {
        btTransform transform;
        transform.setIdentity();
        transform.setOrigin(OgreBulletCollisions::OgreBtConverter::to(mSpawnPoint));;
        mBody->getBulletRigidBody()->clearForces();
        mBody->getBulletRigidBody()->setLinearVelocity(btVector3(0,0,0));
        mBody->getBulletRigidBody()->setAngularVelocity(btVector3(0,0,0));
        mBody->getBulletRigidBody()->setWorldTransform(transform);
    }
    if(mTimer.getMilliseconds()>60.0) {
        mGroundNormal = Vector3::ZERO;
    }
    Actor::update(dt);
}
