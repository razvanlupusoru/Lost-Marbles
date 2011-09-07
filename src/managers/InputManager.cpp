#include "InputManager.h"

template<> InputManager* Ogre::Singleton<InputManager>::ms_Singleton = 0;

InputManager::InputManager(Ogre::RenderWindow* rwindow)
{
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;
	OIS::ParamList pl;

	rwindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
	mInputManager = OIS::InputManager::createInputSystem(pl);

	try
	{
		mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));
		mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject(OIS::OISMouse, true));
		//mJoy = static_cast<OIS::JoyStick*>(mInputManager->createInputObject(OIS::OISJoyStick, false));
	}
	catch (const OIS::Exception &e)
	{
		throw new Ogre::Exception(42, e.eText, "Application::setupInputSystem");
	}

}

InputManager::~InputManager()
{
	mInputManager->destroyInputObject(mKeyboard);
	mInputManager->destroyInputObject(mMouse);
	OIS::InputManager::destroyInputSystem(mInputManager);
}
