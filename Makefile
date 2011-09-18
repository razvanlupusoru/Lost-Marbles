# Settings
CC=g++
AR=ar
CFLAGS=-O3
DEBUG=false
OS=windows
# End Settings

INCLUDEFLAGS= \
	-Idependencies \
	-Idependencies/Bullet \
	-Idependencies/OGRE \
	-Idependencies/OgreBullet/Collisions/include \
	-Idependencies/OgreBullet/Dynamics/include \
	-Idependencies/fmod/include \
	-Idependencies/Hydrax/include \
	-Idependencies/lua/include \
	-Idependencies/luabind/include \
	-Idependencies/MyGUI/MyGUIEngine/include \
	-Idependencies/MyGUI/Platforms/Ogre/OgrePlatform/include \
	-Idependencies/OIS \
	-Isrc/include
CFLAGS += $(INCLUDEFLAGS)

ifeq ($(DEBUG),true)
	CFLAGS += -g -DLostMarblesDebug
endif

ogrebulletobjects = \
	dependencies/OgreBullet/Collisions/src/OgreBulletCollisionsObject.o \
	dependencies/OgreBullet/Collisions/src/OgreBulletCollisionsObjectState.o \
	dependencies/OgreBullet/Collisions/src/OgreBulletCollisionsPrecompiled.o \
	dependencies/OgreBullet/Collisions/src/OgreBulletCollisionsRay.o \
	dependencies/OgreBullet/Collisions/src/OgreBulletCollisionsShape.o \
	dependencies/OgreBullet/Collisions/src/OgreBulletCollisionsWorld.o \
	dependencies/OgreBullet/Collisions/src/Debug/OgreBulletCollisionsDebugContact.o \
	dependencies/OgreBullet/Collisions/src/Debug/OgreBulletCollisionsDebugDrawer.o \
	dependencies/OgreBullet/Collisions/src/Debug/OgreBulletCollisionsDebugLines.o \
	dependencies/OgreBullet/Collisions/src/Debug/OgreBulletCollisionsDebugShape.o \
	dependencies/OgreBullet/Collisions/src/Shapes/OgreBulletCollisionsBoxShape.o \
	dependencies/OgreBullet/Collisions/src/Shapes/OgreBulletCollisionsCapsuleShape.o \
	dependencies/OgreBullet/Collisions/src/Shapes/OgreBulletCollisionsCompoundShape.o \
	dependencies/OgreBullet/Collisions/src/Shapes/OgreBulletCollisionsConeShape.o \
	dependencies/OgreBullet/Collisions/src/Shapes/OgreBulletCollisionsConvexHullShape.o \
	dependencies/OgreBullet/Collisions/src/Shapes/OgreBulletCollisionsCylinderShape.o \
	dependencies/OgreBullet/Collisions/src/Shapes/OgreBulletCollisionsGImpactShape.o \
	dependencies/OgreBullet/Collisions/src/Shapes/OgreBulletCollisionsMinkowskiSumShape.o \
	dependencies/OgreBullet/Collisions/src/Shapes/OgreBulletCollisionsMultiSphereShape.o \
	dependencies/OgreBullet/Collisions/src/Shapes/OgreBulletCollisionsSphereShape.o \
	dependencies/OgreBullet/Collisions/src/Shapes/OgreBulletCollisionsStaticPlaneShape.o \
	dependencies/OgreBullet/Collisions/src/Shapes/OgreBulletCollisionsTriangleShape.o \
	dependencies/OgreBullet/Collisions/src/Shapes/OgreBulletCollisionsTrimeshShape.o \
	dependencies/OgreBullet/Dynamics/src/OgreBulletDynamicsConstraint.o \
	dependencies/OgreBullet/Dynamics/src/OgreBulletDynamicsObjectState.o \
	dependencies/OgreBullet/Dynamics/src/OgreBulletDynamicsPrecompiled.o \
	dependencies/OgreBullet/Dynamics/src/OgreBulletDynamicsRigidBody.o \
	dependencies/OgreBullet/Dynamics/src/OgreBulletDynamicsWorld.o \
	dependencies/OgreBullet/Dynamics/src/Prefab/OgreBulletDynamicsRagDoll.o \
	dependencies/OgreBullet/Dynamics/src/Constraints/OgreBulletDynamics6DofConstraint.o \
	dependencies/OgreBullet/Dynamics/src/Constraints/OgreBulletDynamics6DofSpringConstraint.o \
	dependencies/OgreBullet/Dynamics/src/Constraints/OgreBulletDynamicsConeTwistConstraint.o \
	dependencies/OgreBullet/Dynamics/src/Constraints/OgreBulletDynamicsHingeConstraint.o \
	dependencies/OgreBullet/Dynamics/src/Constraints/OgreBulletDynamicsPoint2pointConstraint.o \
	dependencies/OgreBullet/Dynamics/src/Constraints/OgreBulletDynamicsRaycastVehicle.o

lostmarblesobjects = \
	src/main.o \
	src/actors/BouncePowerUp.o \
	src/actors/CollectibleMarble.o \
	src/actors/CompetitorMarble.o \
	src/actors/FloatPowerUp.o \
	src/actors/HealthPowerUp.o \
	src/actors/HelpPowerUp.o \
	src/actors/SpeedPowerUp.o \
	src/actors/StickyPowerUp.o \
	src/actors/TeleportPowerUp.o \
	src/behaviors/LostBehavior.o \
	src/behaviors/PlayerBehavior.o \
	src/behaviors/RivalBehavior.o \
	src/helpers/OverlayFader.o \
	src/managers/DotLevelLoader.o \
	src/managers/GameManager.o \
	src/managers/InputManager.o \
	src/managers/LuaBinding.o \
	src/managers/SoundManager.o \
	src/managers/TextRenderer.o \
	src/powerups/BouncePowerUpBehavior.o \
	src/powerups/FloatPowerUpBehavior.o \
	src/powerups/SpeedPowerUpBehavior.o \
	src/powerups/StickyPowerUpBehavior.o \
	src/powerups/UnderwaterBehavior.o \
	src/states/BlankState.o \
	src/states/LogoState.o \
	src/states/MainMenuState.o \
	src/states/PauseState.o \
	src/states/PlayState.o \
	src/states/QuitGameState.o \
	src/states/WinState.o

ifeq ($(OS),windows)
	RemoveObjectFiles=Clean.bat
	LOSTMARBLESEXE=bin/LostMarbles.exe
else
	RemoveObjectFiles=rm -r $(objects) $(ogrebulletobjects)
	LOSTMARBLESEXE=bin/LostMarbles
endif	

all: $(ogrebulletobjects) $(lostmarblesobjects)

$(LOSTMARBLESEXE): $(lostmarblesobjects) $(ogrebulletobjects)
	$(CC) $(CFLAGS) $(lostmarblesobjects) $(ogrebulletobjects) -o $@

$(ogrebulletobjects): %.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(lostmarblesobjects): %.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY : clean
clean :
	$(RemoveObjectFiles) 

