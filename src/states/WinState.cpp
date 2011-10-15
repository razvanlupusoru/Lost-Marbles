#include "WinState.h"

using namespace Ogre;

WinState WinState::sWinState;

WinState::~WinState()
{
}
void WinState::enter()
{
    //mTimeElapsed = 0;
    mParticleUpdate = 0;
    mKeyboard = InputManager::getSingletonPtr()->getKeyboard();
    mMouse = InputManager::getSingletonPtr()->getMouse();
    mRoot = Root::getSingletonPtr();
    mSoundMgr = SoundManager::getSingletonPtr();

    mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "WinSMInstance");
    mCamera = mSceneMgr->createCamera("WinCamera");
    mViewport = mRoot->getAutoCreatedWindow()->addViewport(mCamera);
    mViewport->setBackgroundColour(ColourValue(0.0, 0.0, 0.0));

    mCamera->setAspectRatio(Real(mViewport->getActualWidth()) / Real(mViewport->getActualHeight()));

    mCamera->setNearClipDistance(1.0);
    mCamNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("menuCamNode");
    mCamNode->setPosition(Vector3(0,0,100));
    mCamNode->attachObject(mCamera);

    redoParticleSystems();
    mSceneMgr->setAmbientLight(ColourValue(0.6, 0.6, 0.6));

    MyGUI::OgreRenderManager::getInstance().setSceneManager(mSceneMgr);
    mGUI = MyGUI::Gui::getInstancePtr();
    mGUI->setVisiblePointer(false);

    mWinningLayout = MyGUI::LayoutManager::getInstancePtr()->loadLayout("Winning.layout");
}
void WinState::exit()
{
    MyGUI::LayoutManager::getInstancePtr()->unloadLayout(mWinningLayout);
    mWinningLayout.clear();
    MyGUI::OgreRenderManager::getInstancePtr()->setSceneManager(NULL);
    Ogre::RenderWindow *win = mRoot->getAutoCreatedWindow();
    if(win)
        win->removeAllViewports();

    if(mSceneMgr) {
        mRoot->destroySceneManager(mSceneMgr);
    }

}

void WinState::redoParticleSystems()
{
    mSceneMgr->destroyAllParticleSystems();
    String particles[4] = { "Particles/blast", "Particles/blast2", "Particles/erruption", "Particles/flow" };
    for(int i = 0; i < 10; ++i) {
        mParticles[i] = mSceneMgr->createParticleSystem("WinParticles" + i, particles[rand()%4]);
        mParticles[i]->getEmitter(0)->setRepeatDelay(60, 60);
        mSceneNodes[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(rand()%50-25,rand()%50-25,rand()%80-40));
        mSceneNodes[i]->attachObject(mParticles[i]);
        mTimers[i].reset();
    }
}

bool WinState::frameStarted(const Ogre::FrameEvent& evt)
{
    int i = mParticleUpdate;
    if(mTimers[i].getMilliseconds() > (mParticles[i]->getEmitter(0)->getMaxDuration() + mParticles[i]->getEmitter(0)->getMaxTimeToLive())*1000+i*600) {
        String particles[4] = { "Particles/blast", "Particles/blast2", "Particles/erruption", "Particles/flow" };
        //mSceneMgr->getParticleSystem("WinParticles"+i)->getEmitter(0)->setEmissionRate(mSceneMgr->getParticleSystem("WinParticles"+i)->getEmitter(0)->getEmissionRate()*percent);
        mSceneMgr->destroyParticleSystem("WinParticles"+i);
        mParticles[i] = mSceneMgr->createParticleSystem("WinParticles" + i, particles[rand()%4]);
        mParticles[i]->getEmitter(0)->setRepeatDelay(60, 60);
        mSceneNodes[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(rand()%50-25,rand()%50-25,rand()%80-40));
        mSceneNodes[i]->attachObject(mParticles[i]);
        mTimers[i].reset();
    }
    mParticleUpdate < 9 ? mParticleUpdate++ : mParticleUpdate=0;
    /*	if(mTimer.getMilliseconds() > 5000)
    	{
    		redoParticleSystems();
    		mTimer.reset();
    	}
    */
    return true;
}

bool WinState::frameEnded(const Ogre::FrameEvent& evt)
{
    return true;
}

void WinState::pause()
{
    //Win state can not be paused.
}

void WinState::resume()
{
    //Win state can not be paused.
}
bool WinState::keyPressed(const OIS::KeyEvent& e)
{
    if (e.key == OIS::KC_Q || e.key == OIS::KC_ESCAPE || e.key == OIS::KC_SPACE) {
        GameManager::getSingletonPtr()->popState();
    }
    return true;
}

bool WinState::keyReleased(const OIS::KeyEvent& e)
{
    //Win state does not receive user input.
    return true;
}

bool WinState::mouseMoved(const OIS::MouseEvent &e)
{
    //Win state does not receive user input.
    return true;
}

bool WinState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    //Win state does not receive user input.
    return true;
}

bool WinState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    //Win state does not receive user input.
    return true;
}
