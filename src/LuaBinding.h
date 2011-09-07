#ifndef LUABINDING_H
#define LUABINDING_H

#include <Ogre.h>
#include <OIS/OIS.h>

#include "luabind/lua_include.hpp"
#include "luabind/luabind.hpp"

#include "RivalBehavior.h"
#include "Behavior.h"
#include "Actor.h"

// Prototype this before operator.hpp so it can be found for tostring() operator.
//std::ostream& operator<<( std::ostream& stream, const Ogre::Entity ent );
#include "luabind/operator.hpp"

class LuaBinding
{
public:
	LuaBinding(lua_State * L) : mLuaState(L) {}
	void bindLua();
private:
	lua_State * mLuaState;
	void bindVector3();
	void bindColourValue();
	void bindIBehavior();
	void bindRivalBehavior();
	void bindActor();
	void bindOgreBulletDynamicsRigidBody();
	void bindbtCollisionObject();
	void bindSceneNode();
	void bindNode();
	void bindGameState();
	void bindPlayState();
};

#endif