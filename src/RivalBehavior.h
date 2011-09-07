#ifndef RIVALBEHAVIOR_H
#define RIVALBEHAVIOR_H

#include "Behavior.h"

#include "Actor.h"
#include "SoundManager.h"
#include "PlayState.h"

class RivalBehavior : public IBehavior
{
public:
	RivalBehavior(Actor *actor, Ogre::String follow, lua_State *luaState) : mActor(actor), mFollow(follow), mLuaState(luaState) {}

	bool act(float dt);
	void wander();
private:
	lua_State* mLuaState;
	Actor *mActor;
	Ogre::String mFollow;
	Vector3 mLastRandVec;
	Timer mTimer;
	bool mResetDir;
};

#endif