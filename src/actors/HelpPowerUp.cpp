#include "HelpPowerUp.h"
#include "CompetitorMarble.h"
#include "PlayState.h"

HelpPowerUp::HelpPowerUp(Ogre::Entity *ent, OgreBulletDynamics::RigidBody *body, const std::string &layoutFile) : Actor(ent, body)
{
    mLayoutFile = layoutFile;
    if(mLayoutFile.size() < 7 || mLayoutFile.substr(layoutFile.size()-7).compare(".layout")!=0)
        mLayoutFile.append(".layout");
}

bool HelpPowerUp::onCollision(CompetitorMarble *otherActor, btManifoldPoint& cp)
{
    if(otherActor->mNeedsHelp) {
        PlayState * playState = PlayState::getInstance();
        playState->loadHelpLayout(mLayoutFile);
    }
    return false;
}