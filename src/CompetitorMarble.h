#ifndef COMPETITORMARBLE_H
#define COMPETITORMARBLE_H

#include "Actor.h"
#include <deque>

class CompetitorMarble : public Actor
{
public:
	CompetitorMarble(Ogre::Entity *ent, OgreBulletDynamics::RigidBody *body);
	bool onCollision(Actor *otherActor, btManifoldPoint& cp);
	bool onCollision(CompetitorMarble *otherActor, btManifoldPoint& cp);
	bool onCollision(StaticActor *otherActor, btManifoldPoint& cp);

	void update(float dt);

	void addCollectedMarble(const std::string & name);
	std::string takeAwayOneMarble();
	std::vector<std::string> getListOfCollectedMarbles();

	int mHealth;
	bool mNeedsHelp;

	SoundManager* mSoundMgr;
	SceneNode *mBodyNode;

protected:
	bool mRolling, mIsSticky;
	int mRollChannel;

private:
	void resetHealth();
	std::deque<Ogre::Real> speeds;
	Ogre::Real mSpeedSum;
	Ogre::Real mSpeedAvg;
	Ogre::Timer mDropSoundTimer;

	std::vector<std::string> mCollectedMarbles;
	OgreBulletDynamics::RigidBody *mBody;
	Timer mTimer;
};

#endif
