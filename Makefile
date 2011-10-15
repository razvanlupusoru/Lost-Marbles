# Settings
CPP=g++
CC=gcc
DEBUG=false
OS=windows
# End Settings

CFLAGS= \
	-Idependencies \
	-Idependencies/Bullet \
	-Idependencies/Bullet/ConvexDecomposition \
	-Idependencies/OGRE/OgreMain/include \
	-Idependencies/OgreBullet/Collisions/include \
	-Idependencies/OgreBullet/Dynamics/include \
	-Idependencies/fmod/include \
	-Idependencies/Hydrax/include \
	-Idependencies/lua/include \
	-Idependencies/luabind/include \
	-Idependencies/freetype/include \
	-Idependencies/MyGUI/MyGUIEngine/include \
	-Idependencies/MyGUI/Platforms/Ogre/OgrePlatform/include \
	-Idependencies/OIS \
	-Isrc/include \
	-Wall \
	-DMYGUI_USE_FREETYPE

ifeq ($(DEBUG),true)
	CFLAGS += -g -DLostMarblesDebug -O1
else
	CFLAGS += -O3
endif

bulletobjects = \
	dependencies/Bullet/BulletCollision/CollisionDispatch/btSimulationIslandManager.o

bulletconvexdecompositionobjects = \
	dependencies/Bullet/ConvexDecomposition/bestfit.o \
	dependencies/Bullet/ConvexDecomposition/bestfitobb.o \
	dependencies/Bullet/ConvexDecomposition/cd_hull.o \
	dependencies/Bullet/ConvexDecomposition/cd_wavefront.o \
	dependencies/Bullet/ConvexDecomposition/concavity.o \
	dependencies/Bullet/ConvexDecomposition/ConvexBuilder.o \
	dependencies/Bullet/ConvexDecomposition/ConvexDecomposition.o \
	dependencies/Bullet/ConvexDecomposition/fitsphere.o \
	dependencies/Bullet/ConvexDecomposition/float_math.o \
	dependencies/Bullet/ConvexDecomposition/meshvolume.o \
	dependencies/Bullet/ConvexDecomposition/planetri.o \
	dependencies/Bullet/ConvexDecomposition/raytri.o \
	dependencies/Bullet/ConvexDecomposition/splitplane.o \
	dependencies/Bullet/ConvexDecomposition/vlookup.o
	
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
	dependencies/OgreBullet/Collisions/src/Utils/OgreBulletCollisionsMeshToShapeConverter.o \
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

luabindobjects = \
	dependencies/luabind/src/class.o \
	dependencies/luabind/src/class_info.o \
	dependencies/luabind/src/class_registry.o \
	dependencies/luabind/src/class_rep.o \
	dependencies/luabind/src/create_class.o \
	dependencies/luabind/src/error.o \
	dependencies/luabind/src/exception_handler.o \
	dependencies/luabind/src/function.o \
	dependencies/luabind/src/inheritance.o \
	dependencies/luabind/src/link_compatibility.o \
	dependencies/luabind/src/object_rep.o \
	dependencies/luabind/src/open.o \
	dependencies/luabind/src/pcall.o \
	dependencies/luabind/src/scope.o \
	dependencies/luabind/src/stack_content_by_name.o \
	dependencies/luabind/src/weak_ref.o \
	dependencies/luabind/src/wrapper_base.o

luaobjects = \
	dependencies/lua/src/lapi.o \
	dependencies/lua/src/lauxlib.o \
	dependencies/lua/src/lbaselib.o \
	dependencies/lua/src/lcode.o \
	dependencies/lua/src/ldblib.o \
	dependencies/lua/src/ldebug.o \
	dependencies/lua/src/ldo.o \
	dependencies/lua/src/ldump.o \
	dependencies/lua/src/lfunc.o \
	dependencies/lua/src/lgc.o \
	dependencies/lua/src/linit.o \
	dependencies/lua/src/liolib.o \
	dependencies/lua/src/llex.o \
	dependencies/lua/src/lmathlib.o \
	dependencies/lua/src/lmem.o \
	dependencies/lua/src/loadlib.o \
	dependencies/lua/src/lobject.o \
	dependencies/lua/src/lopcodes.o \
	dependencies/lua/src/loslib.o \
	dependencies/lua/src/lparser.o \
	dependencies/lua/src/lstate.o \
	dependencies/lua/src/lstring.o \
	dependencies/lua/src/lstrlib.o \
	dependencies/lua/src/ltable.o \
	dependencies/lua/src/ltablib.o \
	dependencies/lua/src/ltm.o \
	dependencies/lua/src/lundump.o \
	dependencies/lua/src/lvm.o \
	dependencies/lua/src/lzio.o \
	dependencies/lua/src/print.o

freetypeobjects = \
	dependencies/freetype/src/base/ftsystem.o \
	dependencies/freetype/src/base/ftinit.o \
	dependencies/freetype/src/base/ftdebug.o \
	dependencies/freetype/src/base/ftbase.o \
	dependencies/freetype/src/base/ftbbox.o \
	dependencies/freetype/src/base/ftglyph.o \
	dependencies/freetype/src/base/ftbdf.o \
	dependencies/freetype/src/base/ftbitmap.o \
	dependencies/freetype/src/base/ftcid.o \
	dependencies/freetype/src/base/ftfstype.o \
	dependencies/freetype/src/base/ftgasp.o \
	dependencies/freetype/src/base/ftgxval.o \
	dependencies/freetype/src/base/ftlcdfil.o \
	dependencies/freetype/src/base/ftmm.o \
	dependencies/freetype/src/base/ftotval.o \
	dependencies/freetype/src/base/ftpatent.o \
	dependencies/freetype/src/base/ftpfr.o \
	dependencies/freetype/src/base/ftstroke.o \
	dependencies/freetype/src/base/ftsynth.o \
	dependencies/freetype/src/base/fttype1.o \
	dependencies/freetype/src/base/ftwinfnt.o \
	dependencies/freetype/src/base/ftxf86.o \
	dependencies/freetype/src/truetype/truetype.o \
	dependencies/freetype/src/smooth/smooth.o
	
myguiengineobjects = \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_Button.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_Canvas.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_ComboBox.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_DDContainer.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_Edit.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_HScroll.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_ItemBox.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_List.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_ListBox.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_ListCtrl.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_MenuBar.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_MenuCtrl.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_MenuItem.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_Message.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_MultiList.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_PopupMenu.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_Progress.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_ScrollView.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_StaticImage.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_StaticText.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_Tab.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_TabItem.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_VScroll.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_Widget.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_Window.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_EditText.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_MainSkin.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_RawRect.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_RotatingSkin.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_SimpleText.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_SubSkin.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_TileRect.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_LayerItem.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_LayerNode.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_OverlappedLayer.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_RenderItem.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_SharedLayer.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_SharedLayerNode.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_ActionController.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_ControllerEdgeHide.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_ControllerFadeAlpha.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_ControllerPosition.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_Exception.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_Precompiled.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_IWidgetCreator.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_ScrollViewBase.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_ResourceImageSet.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_ResourceImageSetPointer.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_ResourceManualFont.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_ResourceManualPointer.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_ResourceSkin.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_ResourceTrueTypeFont.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_MaskPickInfo.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_Any.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_Colour.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_ClipboardManager.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_ControllerManager.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_DataManager.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_DynLibManager.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_FactoryManager.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_FontManager.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_Gui.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_InputManager.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_LanguageManager.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_LayerManager.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_LayoutManager.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_PluginManager.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_PointerManager.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_RenderManager.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_ResourceManager.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_SkinManager.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_SubWidgetManager.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_WidgetManager.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_DataFileStream.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_DataStream.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_DynLib.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_Guid.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_RenderOut.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_TextIterator.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_TextureUtility.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_Timer.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_XmlDocument.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_LogManager.o \
	dependencies/MyGUI/MyGUIEngine/src/MyGUI_LogStream.o

myguiplatformobjects = \
	dependencies/MyGUI/Platforms/Ogre/OgrePlatform/src/MyGUI_OgreDataManager.o \
	dependencies/MyGUI/Platforms/Ogre/OgrePlatform/src/MyGUI_OgreDataStream.o \
	dependencies/MyGUI/Platforms/Ogre/OgrePlatform/src/MyGUI_OgreRenderManager.o \
	dependencies/MyGUI/Platforms/Ogre/OgrePlatform/src/MyGUI_OgreTexture.o \
	dependencies/MyGUI/Platforms/Ogre/OgrePlatform/src/MyGUI_OgreRTTexture.o \
	dependencies/MyGUI/Platforms/Ogre/OgrePlatform/src/MyGUI_OgreVertexBuffer.o

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

dependencies = \
	$(bulletobjects) \
	$(bulletconvexdecompositionobjects) \
	$(ogrebulletobjects) \
	$(luabindobjects) \
	$(myguiengineobjects) \
	$(myguiplatformobjects)

cdependencies = \
	$(luaobjects) \
	
ifeq ($(OS),windows)
	RemoveObjectFiles=Clean.bat
	LOSTMARBLESEXE=bin/LostMarbles.exe
	LDFLAGS=-Llib/prebuilt/windows-x86 -mwindows -lmingw32 -lwinmm -lole32
else
	RemoveObjectFiles=rm -rf $(lostmarblesobjects) $(dependencies)
	LOSTMARBLESEXE=bin/LostMarbles
endif

LDFLAGS += -Lbin -lOIS -lOgreMain -lPlugin_ParticleFX -lRenderSystem_Direct3D9 -lRenderSystem_GL -lfmodex -lBulletCollision -lBulletDynamics -lLinearMath -lfreetype

all: $(LOSTMARBLESEXE)

$(LOSTMARBLESEXE): $(lostmarblesobjects) $(dependencies) $(cdependencies)
	$(CPP) $(CFLAGS) $(dependencies) $(cdependencies) $(lostmarblesobjects) $(LDFLAGS) -o $@

$(lostmarblesobjects): %.o: %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

$(dependencies): %.o: %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@
	
$(cdependencies): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY : clean
clean :
	$(RemoveObjectFiles) 

