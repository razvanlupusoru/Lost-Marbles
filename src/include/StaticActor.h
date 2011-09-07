#ifndef STATICACTOR_H
#define STATICACTOR_H

#include "Actor.h"

class StaticActor : public Actor
{
public:
	StaticActor(Ogre::Entity *ent, OgreBulletDynamics::RigidBody *body) : Actor(ent, body) {}

	bool onCollision(Actor *otherActor, btManifoldPoint& cp) { return otherActor->onCollision(this, cp); }
};

#endif
