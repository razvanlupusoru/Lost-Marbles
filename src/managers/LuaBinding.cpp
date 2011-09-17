#include "LuaBinding.h"

using namespace luabind;
using namespace Ogre;

// Some helpful macros for defining constants (sort of) in Lua.  Similar to this code:
// object g = globals(L);
// object table = g["class"];
// table["constant"] = class::constant;
#define LUA_CONST_START( class ) { object g = globals(mLuaState); object table = g[#class];
#define LUA_CONST( class, name ) table[#name] = class::name
#define LUA_CONST_END }

void LuaBinding::bindVector3()
{
	module(mLuaState)
	[
		class_<Vector3>( "Vector3" )
		.def(tostring(self))
		.def_readwrite( "x", &Vector3::x )
		.def_readwrite( "y", &Vector3::y )
		.def_readwrite( "z", &Vector3::z )
		.def(constructor<>())
        .def(constructor<Vector3&>())
		.def(constructor<Real, Real, Real>())
		.def("absDotProduct", &Vector3::absDotProduct)
		.def("crossProduct", &Vector3::crossProduct )
		.def("directionEquals", &Vector3::directionEquals )
		.def("distance", &Vector3::distance )
		.def("dotProduct", &Vector3::dotProduct )
		.def("getRotationTo", &Vector3::getRotationTo )
		.def("isZeroLength", &Vector3::isZeroLength )
		.def("length", &Vector3::length )
		.def("makeCeil", &Vector3::makeCeil )
		.def("makeFloor", &Vector3::makeFloor )
		.def("midPoint", &Vector3::midPoint )
		.def("normalise", &Vector3::normalise )
		.def("nornaliseCopy", &Vector3::normalisedCopy )
		.def("perpendicular", &Vector3::perpendicular )
		.def("positionCloses", &Vector3::positionCloses )
		.def("positionEquals", &Vector3::positionEquals )
		//.def("ptr", &Vector3::ptr )
		.def("randomDeviant", &Vector3::randomDeviant )
		.def("reflect", &Vector3::reflect )
		.def("squaredDistance", &Vector3::squaredDistance )
		.def("squaredLength", &Vector3::squaredLength )

		// Operators

		.def( self + other<Vector3>() )
		.def( self - other<Vector3>() )
		.def( self * other<Vector3>() )
		.def( self * Real() )
	];

	LUA_CONST_START( Vector3 )
		LUA_CONST( Vector3, ZERO);
		LUA_CONST( Vector3, UNIT_X );
		LUA_CONST( Vector3, UNIT_X);
		LUA_CONST( Vector3, UNIT_Y);
		LUA_CONST( Vector3, UNIT_Z);
		LUA_CONST( Vector3, NEGATIVE_UNIT_X);
		LUA_CONST( Vector3, NEGATIVE_UNIT_Y);
		LUA_CONST( Vector3, NEGATIVE_UNIT_Z);
		LUA_CONST( Vector3, UNIT_SCALE);
	LUA_CONST_END;
}

void LuaBinding::bindColourValue()
{
	module(mLuaState)
	[
		class_<ColourValue>("ColourValue")
		.def(constructor<>())
		.def(constructor<Real, Real, Real, Real>())
		.def(constructor<Real, Real, Real>())
		.def(tostring(self))
		.def_readwrite( "r", &ColourValue::r)
		.def_readwrite( "g", &ColourValue::g )
		.def_readwrite( "b", &ColourValue::b )
		.def_readwrite( "a", &ColourValue::a )
		.def( "saturate", &ColourValue::saturate )

		// Operators

		.def( self + other<ColourValue>() )
		.def( self - other<ColourValue>() )
		.def( self * other<ColourValue>() )
		.def( self * Real() )
		.def( self / Real() )
	];

	LUA_CONST_START( ColourValue )
		LUA_CONST( ColourValue, ZERO);
		LUA_CONST( ColourValue, Black);
		LUA_CONST( ColourValue, White);
		LUA_CONST( ColourValue, Red);
		LUA_CONST( ColourValue, Green);
		LUA_CONST( ColourValue, Blue);
	LUA_CONST_END;
}

void LuaBinding::bindIBehavior()
{
	module(mLuaState)
	[
		class_<IBehavior>("IBehavior")
		.def("act", &IBehavior::act)
	];
}

void LuaBinding::bindRivalBehavior()
{
	module(mLuaState)
	[
		class_<RivalBehavior, bases<IBehavior> >("RivalBehavior")
		.def("wander", &RivalBehavior::wander)
	];
}

void LuaBinding::bindActor()
{
	module(mLuaState)
	[
		class_<Actor>("Actor")
		.def("getBody", &Actor::getBody)
		.def_readwrite( "score", &Actor::mScore)
	];
}

void LuaBinding::bindOgreBulletDynamicsRigidBody()
{
	module(mLuaState)
	[
		class_<OgreBulletDynamics::RigidBody>("RigidBody")
		.def("getBulletObject", &OgreBulletDynamics::RigidBody::getBulletObject)
		.def("applyForce", &OgreBulletDynamics::RigidBody::applyForce)
		.def("getSceneNode", &OgreBulletDynamics::RigidBody::getSceneNode)
	];
}

void LuaBinding::bindbtCollisionObject()
{
	module(mLuaState)
	[
		class_<btCollisionObject>("btCollisionObject")
		.def("activate", &btCollisionObject::activate)
	];
}

void LuaBinding::bindNode()
{
	module(mLuaState)
	[
		class_<Ogre::Node>("Node")
		.def("getPosition", &Ogre::Node::getPosition)
	];
}

void LuaBinding::bindSceneNode()
{
	module(mLuaState)
	[
		class_<Ogre::SceneNode, bases<Ogre::Node> >("SceneNode")
	];
}

void LuaBinding::bindGameState()
{
	module(mLuaState)
	[
		class_<GameState>("GameState")
		.def("enter", &GameState::enter)
		.def("exit", &GameState::exit)
		.def("pause", &GameState::pause)
		.def("resume", &GameState::resume)
		.def("frameStarted", &GameState::frameStarted)
		.def("frameEnded", &GameState::frameEnded)
	];
}

void LuaBinding::bindPlayState()
{
	module(mLuaState)
	[
		class_<PlayState, GameState>("PlayState")
		.enum_("States")
		[
			value("debug", 0),
			value("normal", 1),
			value("transition", 2)
		]
		.def("getInstance", &PlayState::getInstance)
		.def("getCurrentState", &PlayState::getCurrentState)
		.def("loadSavedGame", &PlayState::loadSavedGame)
	];
}

void LuaBinding::bindLua()
{
	bindVector3();
	bindColourValue();
	bindIBehavior();
	bindRivalBehavior();
	bindActor();
	bindOgreBulletDynamicsRigidBody();
	bindbtCollisionObject();
	bindNode();
	bindSceneNode();
	bindGameState();
	bindPlayState();
}