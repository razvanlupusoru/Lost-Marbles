#ifndef COLLECTIBLEMARBLE_H
#define COLLECTIBLEMARBLE_H

#include "Actor.h"

class CollectibleMarble : public Actor
{
public:
	CollectibleMarble(Ogre::Entity *ent, OgreBulletDynamics::RigidBody *body);
	bool onCollision(Actor *otherActor, btManifoldPoint& cp);
	bool onCollision(CompetitorMarble *otherActor, btManifoldPoint& cp);

	void update(float dt);
protected:
	SoundManager* mSoundMgr;
	SceneNode *mBodyNode;
	int mCryChannel;

	Ogre::Timer mTimer;
};

#endif
