#include "PauseState.h"

#include "utils.h"
#include "FileSystemInfo.h"
#include "PlayState.h"

using namespace Ogre;

PauseState PauseState::sPauseState;

PauseState::~PauseState()
{
}
void PauseState::enter()
{
    mKeyboard = InputManager::getSingletonPtr()->getKeyboard();
    mMouse = InputManager::getSingletonPtr()->getMouse();
    mRoot = Root::getSingletonPtr();
    mSoundMgr = SoundManager::getSingletonPtr();

    mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "PauseSMInstance");
    mCamera = mSceneMgr->createCamera("MainMenuCamera");
    mViewport = mRoot->getAutoCreatedWindow()->addViewport(mCamera);
    mViewport->setBackgroundColour(ColourValue(0.2, 0.2, 0.2));

    mCamera->setAspectRatio(Real(mViewport->getActualWidth()) / Real(mViewport->getActualHeight()));

    mCamera->setNearClipDistance(1.0);
    mCamNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("menuCamNode");
    mCamNode->attachObject(mCamera);

    mSceneMgr->setAmbientLight(ColourValue(0.6, 0.6, 0.6));

    Light *light = mSceneMgr->createLight("Light1");
    light->setType(Light::LT_POINT);
    mCamNode->attachObject(light);
    light->setDiffuseColour(ColourValue::White);
    light->setSpecularColour(ColourValue::White);

    mMiniScreen = new Ogre::Rectangle2D(true);
    mMiniScreen->setCorners(-1.0, 1.0, 1.0, -1.0);
    mMiniScreen->setBoundingBox(AxisAlignedBox(-100000.0*Vector3::UNIT_SCALE, 100000.0*Vector3::UNIT_SCALE));
    Ogre::SceneNode *mMiniScreenNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("mMiniScreenNode");
    mMiniScreenNode->attachObject(mMiniScreen);

    MaterialPtr material = MaterialManager::getSingleton().create("RttMat", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::Technique *technique = material->createTechnique();
    technique->createPass();
    material->getTechnique(0)->getPass(0)->setLightingEnabled(true);
    material->getTechnique(0)->getPass(0)->createTextureUnitState("RttTex");
    //material ->setAmbient(0.95, 0.95, 0.95);
    //material ->setDiffuse(0.9, 0.9, 0.9, 1.0);

    mMiniScreen->setMaterial("RttMat");
    initializeGUI();
}

void PauseState::initializeGUI()
{
    widgetNavigation = NULL;
    widgetNavigation = new MyGUI::WidgetPtr[3];

    MyGUI::OgreRenderManager::getInstancePtr()->setSceneManager(mSceneMgr);
    mGUI = MyGUI::Gui::getInstancePtr();

    mGUI->setVisiblePointer(true);
    mGUI->injectMouseMove(mMouse->getMouseState().X.abs, mMouse->getMouseState().Y.abs, mMouse->getMouseState().Z.abs);

    mPauseLayout = MyGUI::LayoutManager::getInstance().loadLayout("PauseMenu.layout");
    mSaveGameLayout = MyGUI::LayoutManager::getInstance().loadLayout("LevelsWindow.layout");

    widgetNavigation[0] = mGUI->findWidget<MyGUI::StaticText>("PauseResumeGameButton");
    widgetNavigation[0]->eventMouseButtonClick = MyGUI::newDelegate(this, &PauseState::handleResumeGameButton);
    widgetNavigation[0]->eventMouseSetFocus = MyGUI::newDelegate(this, &PauseState::handleMouseSetFocusOnButton);
    widgetNavigation[0]->eventMouseLostFocus = MyGUI::newDelegate(this, &PauseState::handleMouseLostFocusOnButton);

    widgetNavigation[2] = mGUI->findWidget<MyGUI::StaticText>("PauseExitGameButton");
    widgetNavigation[2]->eventMouseButtonClick = MyGUI::newDelegate(this, &PauseState::handleExitGameButton);
    widgetNavigation[2]->eventMouseSetFocus = MyGUI::newDelegate(this, &PauseState::handleMouseSetFocusOnButton);
    widgetNavigation[2]->eventMouseLostFocus = MyGUI::newDelegate(this, &PauseState::handleMouseLostFocusOnButton);

    widgetNavigation[1] = mGUI->findWidget<MyGUI::StaticText>("PauseResetLevelButton");
    widgetNavigation[1]->eventMouseButtonClick = MyGUI::newDelegate(this, &PauseState::handleResetLevelButton);
    widgetNavigation[1]->eventMouseSetFocus = MyGUI::newDelegate(this, &PauseState::handleMouseSetFocusOnButton);
    widgetNavigation[1]->eventMouseLostFocus = MyGUI::newDelegate(this, &PauseState::handleMouseLostFocusOnButton);

    //MyGUI::WindowPtr window = mGUI->findWidget<MyGUI::Window>("LoadSaveFileWindow");
    //window->eventWindowButtonPressed = MyGUI::newDelegate(this, &PauseState::handleCancelWindow);
    //window->setCaption("Save Game");

    mWidgetFocus = 0;
    setFocusWidget(widgetNavigation[mWidgetFocus]);

    //MyGUI::ButtonPtr button = mGUI->findWidget<MyGUI::Button>("LoadSaveFileWindow/LoadSaveButton");
    //button->eventMouseButtonClick = MyGUI::newDelegate(this, &PauseState::handleSaveButtonPress);
    //button->setCaption("Save Game");

    //MyGUI::ListPtr list = mGUI->findWidget<MyGUI::List>("LoadSaveFileWindow/ListFiles");
    //list->eventListChangePosition = MyGUI::newDelegate(this, &PauseState::handleNotifyListChangePosition);
    /*
    common::VectorFileInfo infos;
    std::string lostMarblesDir(utils::getLostMarblesWriteDir());
    std::wstring lostMarblesDirWide;
    lostMarblesDirWide.assign(lostMarblesDir.begin(), lostMarblesDir.end());
    common::getSystemFileList(infos, lostMarblesDirWide, L"*.lost");
    for(common::VectorFileInfo::iterator item=infos.begin(); item!=infos.end(); ++item)
    {
    	if (!(*item).folder)
    		list->addItem((*item).name, *item);
    }
    */
    //MyGUI::EditPtr editFilename = mGUI->findWidget<MyGUI::Edit>("LoadSaveFileWindow/EditFileName");
    //editFilename->eventEditSelectAccept = MyGUI::newDelegate(this, &PauseState::handleEditAccept);

    //reset default mouse position
    OIS::MouseState ms = mMouse->getMouseState();
    mGUI->injectMouseMove(ms.width/2,ms.height/2,0);
    OIS::MouseState &mutableMouseState = const_cast<OIS::MouseState &>(mMouse->getMouseState());
    mutableMouseState.X.abs = ms.width/2;
    mutableMouseState.Y.abs = ms.height/2;
}

void PauseState::exit()
{
    MyGUI::LayoutManager::getInstance().unloadLayout(mPauseLayout);
    MyGUI::LayoutManager::getInstance().unloadLayout(mSaveGameLayout);
    MyGUI::OgreRenderManager::getInstancePtr()->setSceneManager(NULL);

    delete [] widgetNavigation;  // When done, free memory pointed to by a.
    widgetNavigation = NULL;     // Clear a to prevent using invalid memory reference.

    Ogre::RenderWindow *win = mRoot->getAutoCreatedWindow();
    if(win)
        win->removeAllViewports();

    MaterialManager::getSingletonPtr()->remove("RttMat");
    delete mMiniScreen;
    if(mSceneMgr) {
        mRoot->destroySceneManager(mSceneMgr);
    }
}

bool PauseState::frameStarted(const Ogre::FrameEvent& evt)
{
    return true;
}

bool PauseState::frameEnded(const Ogre::FrameEvent& evt)
{
    return true;
}

void PauseState::pause()
{
}

void PauseState::resume()
{
}
bool PauseState::keyPressed(const OIS::KeyEvent& e)
{
    /*
    if(mGUI->findWidget<MyGUI::Window>("LoadSaveFileWindow")->isVisible())
    {
    	mGUI->injectKeyPress(MyGUI::KeyCode::Enum(e.key), e.text);
    }*/
    if( e.key == OIS::KC_RETURN) {
        widgetNavigation[mWidgetFocus]->eventMouseButtonClick(widgetNavigation[mWidgetFocus]);
    } else if ( e.key == OIS::KC_UP || e.key == OIS::KC_W) {
        mWidgetFocus--;
        if (mWidgetFocus < 0) {
            mWidgetFocus = 0;
        } else {
            setFocusWidget(widgetNavigation[mWidgetFocus]);
            lostFocusWidget(widgetNavigation[mWidgetFocus+1]);
        }
    } else if ( e.key == OIS::KC_DOWN || e.key == OIS::KC_S) {
        mWidgetFocus++;
        if (mWidgetFocus > 2) {
            mWidgetFocus = 2;
        } else {
            setFocusWidget(widgetNavigation[mWidgetFocus]);
            lostFocusWidget(widgetNavigation[mWidgetFocus-1]);
        }
    } else if ( e.key == OIS::KC_ESCAPE || e.key == OIS::KC_P) {
        GameManager::getSingletonPtr()->popState();
    }

    return true;
}

bool PauseState::keyReleased(const OIS::KeyEvent& e)
{
    /*
    if(mGUI->findWidget<MyGUI::Window>("LoadSaveFileWindow")->isVisible())
    {
    	mGUI->injectKeyRelease(MyGUI::KeyCode::Enum(e.key));
    }
    */
    return true;
}

bool PauseState::mouseMoved(const OIS::MouseEvent &e)
{
    if(mGUI)
        mGUI->injectMouseMove(e.state.X.abs, e.state.Y.abs, e.state.Z.abs);
    return true;
}

bool PauseState::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    if(mGUI)
        mGUI->injectMousePress(e.state.X.abs, e.state.Y.abs, MyGUI::MouseButton::Enum(id));
    return true;
}

bool PauseState::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    if(mGUI)
        mGUI->injectMouseRelease(e.state.X.abs, e.state.Y.abs, MyGUI::MouseButton::Enum(id));
    return true;
}

void PauseState::handleExitGameButton(MyGUI::WidgetPtr _sender)
{
    PlayState * playState = PlayState::getInstance();
    playState->saveGame();
    GameManager::getSingletonPtr()->popState();
    GameManager::getSingletonPtr()->popState();
}

void PauseState::handleResumeGameButton(MyGUI::WidgetPtr _sender)
{
    GameManager::getSingletonPtr()->popState();
}

void PauseState::handleResetLevelButton(MyGUI::WidgetPtr _sender)
{
    handleResumeGameButton(_sender);
    PlayState * playState = PlayState::getInstance();
    playState->resetLevel();
    //playState->saveGame();
    //MyGUI::WindowPtr saveWindow = mGUI->findWidget<MyGUI::Window>("LoadSaveFileWindow");
    //saveWindow->setVisibleSmooth(true);
    //MyGUI::EditPtr editFilename = mGUI->findWidget<MyGUI::Edit>("LoadSaveFileWindow/EditFileName");
    //MyGUI::InputManager::getInstance().setKeyFocusWidget(editFilename);
}

void PauseState::handleMouseSetFocusOnButton(MyGUI::WidgetPtr _sender, MyGUI::WidgetPtr _old)
{
    int i = 0;
    for (i = 0; i < 3 ; i++) {
        if (widgetNavigation[i] == _sender) {
            mWidgetFocus = i;
            setFocusWidget(_sender);
        } else
            lostFocusWidget(widgetNavigation[i]);
    }
}

void PauseState::handleMouseLostFocusOnButton(MyGUI::WidgetPtr _sender, MyGUI::WidgetPtr _new)
{
}

void PauseState::handleCancelWindow(MyGUI::WidgetPtr _widget, const std::string& _name)
{
    if (_name == "close") {
        MyGUI::WindowPtr window = MyGUI::castWidget<MyGUI::Window>(_widget);
        window->setVisibleSmooth(false);
    }
}

void PauseState::setFocusWidget(MyGUI::WidgetPtr _sender)
{
    int soundChannel = -1;
    mSoundMgr->PlaySound("hover", NULL, &soundChannel);
    _sender->setTextColour(MyGUI::Colour(0.31, 0.73, 1, 1));
}
void PauseState::lostFocusWidget(MyGUI::WidgetPtr _sender)
{
    _sender->setTextColour(MyGUI::Colour(1, 1, 1, 1));
}

void PauseState::handleNotifyListChangePosition(MyGUI::List* _sender, size_t _index)
{
    /*
    MyGUI::EditPtr editName = mGUI->findWidget<MyGUI::Edit>("LoadSaveFileWindow/EditFileName");
    if (_index == MyGUI::ITEM_NONE)
    {
    	editName->setCaption("");
    }
    else
    {
    	common::FileInfo info = *_sender->getItemDataAt<common::FileInfo>(_index);
    	if (!info.folder)
    		editName->setCaption(info.name);
    }*/
}

void PauseState::handleSaveButtonPress(MyGUI::WidgetPtr _sender)
{
    //if(_sender->getName()=="LoadSaveFileWindow/LoadSaveButton")
    //{
    //	PlayState * playState = PlayState::getInstance();
//		playState->saveGame();
    /*
    MyGUI::EditPtr editName = mGUI->findWidget<MyGUI::Edit>("LoadSaveFileWindow/EditFileName");
    if(editName->getCaption().length() > 0)
    {
    	std::string name;
    	name.assign(editName->getCaption().asWStr().begin(),editName->getCaption().asWStr().end());
    	playState->saveGame(name);
    }
    MyGUI::WindowPtr window = mGUI->findWidget<MyGUI::Window>("LoadSaveFileWindow");
    window->setVisibleSmooth(false);
    */
    //}
}

void PauseState::handleEditAccept(MyGUI::Edit* _sender)
{
    MyGUI::ButtonPtr button = mGUI->findWidget<MyGUI::Button>("LoadSaveFileWindow/LoadSaveButton");
    handleSaveButtonPress(button);
}