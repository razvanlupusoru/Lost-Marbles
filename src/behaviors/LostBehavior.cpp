#include "LostBehavior.h"

using namespace Ogre;

bool LostBehavior::act(float dt)
{
    if(mJumpTimer.getMilliseconds() > 1000.0f/60.0f) {
        mJumpTimer.reset();
        mActor->getBody()->getBulletObject()->activate();
        mActor->getBody()->applyImpulse(mActor->groundNormal()*0.000000002f,Vector3(0, 0, 0));
    }
    return false;
}