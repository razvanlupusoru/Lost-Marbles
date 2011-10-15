#include "RivalBehavior.h"
#include <OgreStringConverter.h>
#include "luabind/lua_include.hpp"
#include "luabind/luabind.hpp"
#include "luahelperfunctions.h"

using namespace Ogre;

void RivalBehavior::wander()
{
    //TODO: don't hard code this
    int radius=1;

    //get position of ball
    Vector3 pos = mActor->getEntity()->getParentNode()->getPosition();
    //and the direction it's rolling
    Vector3 dir = mActor->getBody()->getLinearVelocity().normalisedCopy();

    Vector3 playerTerrainLocation = Vector3(pos.x,pos.y-radius,pos.z);


    //the center of the circle in front of the marble on which we'll pick a spot
    Vector3 center = pos + dir*10;
    Vector3 focusPoint;
    if(mTimer.getMilliseconds() > 2500 || mResetDir) {
        //Define our point of focus on the imaginary circle around the marble
        float rx = (double)rand()/((double)(RAND_MAX))*10.0f-5.0f;
        float ry = 0.0f;
        float rz = (double)rand()/((double)(RAND_MAX))*10.0f-5.0f;
        //Move the circle to the point we picked before in front of the marble
        focusPoint = center + Vector3(rx,ry,rz);
        mLastRandVec=focusPoint;
        mTimer.reset();
    } else {
        focusPoint=mLastRandVec;
    }

    //btVector3 origin(focusPoint.x,focusPoint.y+5,focusPoint.z);
    Vector3 tempOrigin=center;
    btVector3 origin(tempOrigin.x,20,tempOrigin.z);
    btVector3 target(tempOrigin.x,-50,tempOrigin.z);
    btCollisionWorld::ClosestRayResultCallback result(origin,target);

    //Test for edge
    mActor->getBody()->getBulletCollisionWorld()->rayTest(origin,target,result);
    Vector3 newTerrainLocation;
    bool keepDirection = false;
    if(result.hasHit() && result.m_collisionObject->isStaticObject()) {
        keepDirection=true;
        newTerrainLocation = Vector3(result.m_hitPointWorld);
    }
    //Test for wall
    origin=btVector3(pos.x,pos.y+radius+1,pos.z);
    center=pos+dir*2;
    target=btVector3(center.x,pos.y+radius+1,center.z);
    btCollisionWorld::ClosestRayResultCallback result2(origin,target);
    mActor->getBody()->getBulletCollisionWorld()->rayTest(origin,target,result2);
    if(result2.hasHit() && result2.m_collisionObject->isStaticObject()) {
        keepDirection=false;
    }


    //If we've decided we need a new direction
    if(!keepDirection) {
        mActor->getBody()->getBulletObject()->activate();
        //there's a small cliff here, don't do it!
        //TODO: Make this newly picked vector a little more intelligent :-P
        mActor->getBody()->applyForce((-focusPoint+pos).normalisedCopy()*20,Vector3(0,0.05,0));
        mResetDir=true;
    } else {
        mActor->getBody()->getBulletObject()->activate();
        mActor->getBody()->applyForce((focusPoint-pos).normalisedCopy()*20,Vector3(0,0.05,0));

    }
}

bool RivalBehavior::act(float dt)
{
    RunLuaScript(mLuaState,"../media/lua_scripts/RivalBehavior.lua");

    try {

        Vector3 playerToRival = mActor->getEntity()->_getManager()->getEntity(mFollow)->getParentSceneNode()->getPosition() - mActor->getBody()->getSceneNode()->getPosition();
        luabind::call_function<void>(mLuaState, "act_rb", this, mActor, playerToRival);
    } catch(Ogre::Exception&) {
        luabind::call_function<void>(mLuaState, "wander_rb", this);
    }

    return false;
}