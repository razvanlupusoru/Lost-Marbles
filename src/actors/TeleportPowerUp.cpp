#include "TeleportPowerUp.h"
#include "CompetitorMarble.h"

TeleportPowerUp::TeleportPowerUp(Ogre::Entity *ent, OgreBulletDynamics::RigidBody *body, Ogre::String target) : Actor(ent, body)
{
    mTarget = target;
    mSoundMgr = SoundManager::getSingletonPtr();
    mTimer.reset();
}

bool TeleportPowerUp::onCollision(CompetitorMarble *otherActor, btManifoldPoint& cp)
{
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(OgreBulletCollisions::OgreBtConverter::to(mEntity->_getManager()->getSceneNode(mTarget)->getPosition()));;
    otherActor->getBody()->getBulletRigidBody()->clearForces();
    otherActor->getBody()->getBulletRigidBody()->setLinearVelocity(btVector3(0,0,0));
    otherActor->getBody()->getBulletRigidBody()->setAngularVelocity(btVector3(0,0,0));
    otherActor->getBody()->getBulletRigidBody()->setWorldTransform(transform);

    if(mTimer.getMilliseconds() > 500) {
        int soundChannel = -1;
        mSoundMgr->PlaySound("teleport", otherActor->mBodyNode, &soundChannel);
        mTimer.reset();
    }

    return false;
}
