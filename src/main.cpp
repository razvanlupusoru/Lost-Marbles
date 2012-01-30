#include <Ogre.h>
#include "utils.h"
#include "GameManager.h"
#include "LogoState.h"
#include <boost/filesystem.hpp>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include <windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
int main(int argc, char **argv)
#endif
{
    int ret = 0;
    Ogre::Log * log = NULL;
    Ogre::Root * root = NULL;
    GameManager* game = NULL;
    Ogre::LogManager * logMgr = NULL;
    bool configCreated = false;
    std::string ogreLogPath, ogreCfgPath;
    
    srand(time(NULL));
    
    using namespace boost::filesystem;
    // Creates the Lost Marbles directory to write to
    path lostMarblesDir(utils::getLostMarblesWriteDir());
    
    try {
        if(! exists(lostMarblesDir))
            create_directories(lostMarblesDir);
    } catch(const filesystem_error& ex) {
        #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        MessageBoxA(NULL, ex.what(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
        #else
        std::cerr << "An exception has occured: " << ex.what() << std::endl;
        #endif
        ret = 1;
        goto gracefulexit;
    }
    

    ogreLogPath = (lostMarblesDir / "Ogre.log").string();
    logMgr = new Ogre::LogManager();
    log = logMgr->createLog(ogreLogPath, true, true, false);
    
    ogreCfgPath = (lostMarblesDir / "Ogre.cfg").string();
    root = new Ogre::Root("Plugins.cfg",ogreCfgPath);

    log->logMessage("Main Lost Marbles write directory " + lostMarblesDir.string());
    log->logMessage("Log Path: " + ogreLogPath);
    log->logMessage("Config Path: " + ogreCfgPath);

    if(!root->restoreConfig()) {
        configCreated = root->showConfigDialog();
        root->saveConfig();
    } else
        configCreated = true;

    if(configCreated) {
        game = new GameManager();

        try {
            // initialize the game and switch to the first state
            game->start(LogoState::getInstance());
        } catch (Ogre::Exception& e) {
            #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBoxA(NULL, e.getFullDescription().c_str(), "An exception has occurred!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
            #else
            std::cerr << "An exception has occured: " << e.getFullDescription() << std::endl;
            #endif
            ret = 1;
            goto gracefulexit;
        } 
    }

gracefulexit:
    if(game) delete game;
    if(root) delete root;
    if(logMgr) delete logMgr;
    return ret;
}
