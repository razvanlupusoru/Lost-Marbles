cmake_minimum_required(VERSION 2.6)

project(LostMarblesGame)

set(DEPENDENCIES_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies)
set(BULLET_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies/Bullet)
set(BULLETCONVEX_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies/Bullet/ConvexDecomposition)
set(OGREMAIN_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies/OGRE/OgreMain/include)
set(OGREBULLETCOLLISIONS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies/OgreBullet/Collisions/include)
set(OGREBULLETDYNAMICS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies/OgreBullet/Dynamics/include)
set(FMOD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies/fmod/include)
set(HYDRAX_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies/Hydrax/include)
set(LUA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies/lua/include)
set(LUABIND_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies/luabind/include)
set(FREETYPE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies/freetype/include)
set(MYGUIENGINE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies/MyGUI/MyGUIEngine/include)
set(MYGUIPLATFORM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies/MyGUI/Platforms/Ogre/OgrePlatform/include)
set(OIS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/dependencies/ois/includes)
set(LOSTMARBLES_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/src/include)

include_directories( ${DEPENDENCIES_INCLUDE_DIR}
    ${BULLET_INCLUDE_DIR}
    ${BULLETCONVEX_INCLUDE_DIR}
    ${OGREMAIN_INCLUDE_DIR}
    ${OGREBULLETCOLLISIONS_INCLUDE_DIR}
    ${OGREBULLETDYNAMICS_INCLUDE_DIR}
    ${FMOD_INCLUDE_DIR}
    ${HYDRAX_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
    ${LUABIND_INCLUDE_DIR}
    ${FREETYPE_INCLUDE_DIR}
    ${MYGUIENGINE_INCLUDE_DIR}
    ${MYGUIPLATFORM_INCLUDE_DIR}
    ${OIS_INCLUDE_DIR}
    ${LOSTMARBLES_INCLUDE_DIR}
)
    

add_executable(lostmarbles
	src/main.cpp
	src/actors/BouncePowerUp.cpp
	src/actors/CollectibleMarble.cpp
	src/actors/CompetitorMarble.cpp
	src/actors/FloatPowerUp.cpp
	src/actors/HealthPowerUp.cpp
	src/actors/HelpPowerUp.cpp
	src/actors/SpeedPowerUp.cpp
	src/actors/StickyPowerUp.cpp
	src/actors/TeleportPowerUp.cpp
	src/behaviors/LostBehavior.cpp
	src/behaviors/PlayerBehavior.cpp
	src/behaviors/RivalBehavior.cpp
	src/helpers/OverlayFader.cpp
	src/managers/DotLevelLoader.cpp
	src/managers/GameManager.cpp
	src/managers/InputManager.cpp
	src/managers/LuaBinding.cpp
	src/managers/SoundManager.cpp
	src/managers/TextRenderer.cpp
	src/powerups/BouncePowerUpBehavior.cpp
	src/powerups/FloatPowerUpBehavior.cpp
	src/powerups/SpeedPowerUpBehavior.cpp
	src/powerups/StickyPowerUpBehavior.cpp
	src/powerups/UnderwaterBehavior.cpp
	src/states/BlankState.cpp
	src/states/LogoState.cpp
	src/states/MainMenuState.cpp
	src/states/PauseState.cpp
	src/states/PlayState.cpp
	src/states/QuitGameState.cpp
	src/states/WinState.cpp)