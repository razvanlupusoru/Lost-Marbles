#include <Ogre.h>
#include "utils.h"
#include "GameManager.h"
#include "LogoState.h"
#include <boost/filesystem.hpp>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include <windows.h>
#include <direct.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
int main(int argc, char **argv)
#endif
{
    srand(time(NULL));

    // Creates the Lost Marbles directory to write to
    boost::filesystem::create_directories(boost::filesystem::path(utils::getSavedGamesDir()));
     boost::filesystem::create_directories(boost::filesystem::path(utils::getLostMarblesWriteDir()));

    std::string ogreLogPath = utils::getLostMarblesWriteDir() + "\\Ogre.log";
    Ogre::LogManager * logMgr = new LogManager;
    Log * log = LogManager::getSingletonPtr()->createLog(ogreLogPath, true, true, false);

    std::string ogreCfgPath = utils::getLostMarblesWriteDir() + "\\Ogre.cfg";
    Ogre::Root * root = new Ogre::Root("Plugins.cfg",ogreCfgPath);

    log->logMessage("Main Lost Marbles write directory " + utils::getLostMarblesWriteDir());
    log->logMessage("Log Path: " + ogreLogPath);
    log->logMessage("Config Path: " + ogreCfgPath);

    bool configCreated = false;
    if(!root->restoreConfig()) {
        configCreated = root->showConfigDialog();
        root->saveConfig();
    } else
        configCreated = true;

    if(configCreated) {
        GameManager* game = new GameManager();

        try {
            // initialize the game and switch to the first state
            game->start(LogoState::getInstance());
        } catch (Ogre::Exception& e) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBoxA(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " << e.getFullDescription();
#endif
        }

        delete game;
    }

    delete log;
    return 0;
}
