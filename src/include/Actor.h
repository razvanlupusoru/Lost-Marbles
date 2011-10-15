#ifndef ACTOR_H
#define ACTOR_H

#include <Ogre.h>
#include <OgreBulletDynamicsRigidBody.h>
#include <vector>

#include "utils.h"

#include "PowerUp.h"
#include "Behavior.h"
#include "SoundManager.h"

class CompetitorMarble;
class StaticActor;
class CollectibleMarble;
class HealthPowerUp;

class Actor {
public:
    Actor(Ogre::Entity *ent, OgreBulletDynamics::RigidBody *body, Ogre::Real maxSpeed = -1) : mEntity(ent), mBody(body), mBehavior(NULL), mFlaggedForRemoval(false), mMaxSpeed(maxSpeed) {
        mBody->getBulletRigidBody()->setCollisionFlags(mBody->getBulletRigidBody()->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
        body->getBulletObject()->setUserPointer(this);
        mGroundNormal = Ogre::Vector3::ZERO;
        mSpawnPoint = mEntity->getParentSceneNode()->getPosition();
    }

    ~Actor() {
        clearBehaviors();
    }

    Ogre::Entity* getEntity() {
        return mEntity;
    }
    OgreBulletDynamics::RigidBody* getBody() {
        return mBody;
    }
    //This will pass ownership of behavior to the Actor (hence the delete)
    void addBehavior(IBehavior *behavior) {
        /*if(mBehavior)
        {
        	delete mBehavior;
        }
        mBehavior = behavior;*/
        mBehavior.push_back(behavior);
    }

    Ogre::Real getMaxSpeed() {
        return mMaxSpeed;
    }
    void setMaxSpeed(Ogre::Real maxSpeed) {
        mMaxSpeed = maxSpeed;
    }

    void addPowerUp(String type, IPowerUp *powerup) {
        if(mPowerUps.find(type) != mPowerUps.end()) {
            mPowerUps[type]->reset();
        } else {
            mPowerUps[type] = powerup;
            powerup->start();
        }
    }

    void removeAllPowerUps() {
        for(std::map<String, IPowerUp*>::iterator i = mPowerUps.begin(); i != mPowerUps.end(); ++i) {
            IPowerUp* p = i->second;
            p->finish();
            delete p;
            i->second = NULL;
        }
        mPowerUps.clear();
    }

    std::map<String, IPowerUp*> getListOfPowerUps() {
        std::map<String, IPowerUp*> listOfPowerUps;
        for(std::map<String, IPowerUp*>::iterator i = mPowerUps.begin(); i != mPowerUps.end(); i++) {
            String s = i->first;
            IPowerUp* p = i->second;
            listOfPowerUps[s] = p;
        }
        return listOfPowerUps;
    }

    void clearBehaviors() {
        while(mBehavior.size() > 0) {
            delete mBehavior.back();
            mBehavior.pop_back();
        }
    }

    virtual void update(float dt) {
        /*if(mBehavior)
        {
        	mBehavior->update(dt);
        }*/
        for(std::vector<IBehavior*>::iterator i = mBehavior.begin(); i != mBehavior.end();) {
            if((*i)->act(dt)) {
                IBehavior* b = *i;
                i = mBehavior.erase(i);
                delete b;
            } else {
                i++;
            }
        }
        std::map<String, IPowerUp*>::iterator i = mPowerUps.begin();
        while(i != mPowerUps.end()) {
            if(i->second->update(dt)) {
                IPowerUp* p = i->second;
                p->finish();
                delete p;
                mPowerUps.erase(i);
                i=mPowerUps.begin();
            } else {
                ++i;
            }
        }
    }

    virtual void internalTick(float dt) {
        if(mMaxSpeed >= 0) {
            btVector3 myVelocity = mBody->getBulletRigidBody()->getLinearVelocity();
            btVector3 velocity = btVector3(myVelocity.x(), 0.0, myVelocity.z());
            btScalar speed = velocity.length();
            if(speed > mMaxSpeed) {
                velocity *= mMaxSpeed/speed;
                mBody->getBulletRigidBody()->setLinearVelocity(btVector3(velocity.x(), myVelocity.y(), velocity.z()));
            }
        }
    }
    //Double dispatch design pattern
    virtual bool onCollision(Actor *otherActor, btManifoldPoint& cp) = 0;
    virtual bool onCollision(CompetitorMarble *otherActor, btManifoldPoint& cp) {
        return false;
    }
    virtual bool onCollision(StaticActor *otherActor, btManifoldPoint& cp) {
        return false;
    }
    virtual bool onCollision(CollectibleMarble *otherActor, btManifoldPoint& cp) {
        return false;
    }
    virtual bool onCollision(HealthPowerUp *otherActor, btManifoldPoint& cp) {
        return false;
    }

    bool removeMe() {
        return mFlaggedForRemoval;
    }

    // TODO: This is bad practice. We should not be accessing instance variables publicly
    // However, for now I am writing the Lua bindings to match this. When this gets updated,
    // the lua bindings and the lua script must be updated...
    int mScore;

    Ogre::Vector3 groundNormal() {
        return mGroundNormal;
    }
    Ogre::Vector3 getSpawn() {
        return mSpawnPoint;
    }

protected:
    Ogre::Entity *mEntity;
    OgreBulletDynamics::RigidBody *mBody;
    bool mFlaggedForRemoval;
    Vector3 mSpawnPoint;
    Ogre::Real mMaxSpeed;
    Ogre::Vector3 mGroundNormal;

    std::vector<IBehavior*> mBehavior;
    std::map<String, IPowerUp*> mPowerUps;
};

#endif
