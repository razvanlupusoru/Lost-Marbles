#ifndef HELPPOWERUP_H
#define HELPPOWERUP_H

#include "Actor.h"

class HelpPowerUp : public Actor
{
public:
	HelpPowerUp(Ogre::Entity *ent, OgreBulletDynamics::RigidBody *body, const std::string &layoutFile);
	bool onCollision(Actor *otherActor, btManifoldPoint& cp) { return otherActor->onCollision(this, cp); }
	bool onCollision(CompetitorMarble *otherActor, btManifoldPoint& cp);
private:
	std::string mLayoutFile;
};

#endif