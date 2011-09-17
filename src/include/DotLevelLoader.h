#ifndef DOT_LEVELLOADER_H
#define DOT_LEVELLOADER_H

// Includes
#include <OgreString.h>
#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include <OgreResourceGroupManager.h>
#include <vector>
#include <OgreBulletDynamicsWorld.h>
#include <OgreBulletCollisionsShape.h>
#include <OgreBulletDynamicsRigidBody.h>
#include <Shapes/OgreBulletCollisionsSphereShape.h>
#include <Shapes/OgreBulletCollisionsBoxShape.h>
#include <Shapes/OgreBulletCollisionsCylinderShape.h>
#include <Shapes/OgreBulletCollisionsConvexHullShape.h>
#include <Shapes/OgreBulletCollisionsCompoundShape.h>
#include <Shapes/OgreBulletCollisionsTrimeshShape.h>
#include <Utils/OgreBulletCollisionsMeshToShapeConverter.h>

#include "luabind/lua_include.hpp"
#include "luabind/luabind.hpp"

#include "rapidxml.hpp"
class Actor;
class CompetitorMarble;
class CollectibleMarble;
class IBehavior;

struct LevelInformation
{
	Ogre::String filename;
	Ogre::String levelname;
	int number;
	Ogre::String music;
};

namespace Ogre
{
	// Forward declarations
	class SceneManager;
	class SceneNode;

	class nodeProperty
	{
	public:
		String nodeName;
		String propertyNm;
		String valueName;
		String typeName;

		nodeProperty(const String &node, const String &propertyName, const String &value, const String &type)
			: nodeName(node), propertyNm(propertyName), valueName(value), typeName(type) {}
	};

	class DotLevelLoader
	{
	public:
		DotLevelLoader(lua_State* luaState) : mSceneMgr(0), mLuaState(luaState) {}
		virtual ~DotLevelLoader() {}

		class FileLocator : public Ogre::ResourceGroupManager
		{
		public:
			FileLocator() {}
			~FileLocator() {}

			Ogre::Archive *Find(Ogre::String &filename)
			{
				ResourceGroup* grp = getResourceGroup("General");
				if (!grp)
					OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND, "Cannot locate a resource group called 'General'", "ResourceGroupManager::openResource");

				OGRE_LOCK_MUTEX(grp->OGRE_AUTO_MUTEX_NAME) // lock group mutex
					ResourceLocationIndex::iterator rit = grp->resourceIndexCaseSensitive.find(filename);
				if (rit != grp->resourceIndexCaseSensitive.end())
				{
					// Found in the index
					Ogre::Archive *fileArchive = rit->second;
					filename = fileArchive->getName() + "/" + filename;
					return fileArchive;
				}
				return NULL;
			}
		};

		void parseLevels(const String &FileName, std::vector<LevelInformation> &levels);
		void parseDotLevel(const String &SceneName, const String &groupName, SceneManager *yourSceneMgr, SceneNode *pAttachNode = NULL, const String &sPrependNode = "");
		void parseActors(const String &FileName, const String &groupName, SceneManager *yourSceneMgr, OgreBulletDynamics::DynamicsWorld *yourBulletWorld, std::map<String, Actor*> &actors, std::vector<CompetitorMarble*> &friends, std::vector<CompetitorMarble*> &rivals, std::vector<CollectibleMarble*> &targets, CompetitorMarble **focus, SceneNode *pAttachNode = NULL, const String &sPrependNode = "");
		void reloadNodesAndActors(const String &dotLevelFileName, const String &actorFileName, const std::vector<std::string> &listToRestore, const String &groupName, SceneManager *yourSceneMgr, OgreBulletDynamics::DynamicsWorld *yourBulletWorld, std::map<String, Actor*> &actors, std::vector<CompetitorMarble*> &friends, std::vector<CompetitorMarble*> &rivals, std::vector<CollectibleMarble*> &targets, CompetitorMarble **focus);
		String getProperty(const String &ndNm, const String &prop);

		std::vector<nodeProperty> nodeProperties;
		std::vector<String> staticObjects;
		std::vector<String> dynamicObjects;

	protected:
		void processScene(rapidxml::xml_node<>* XMLRoot);
		void processNodes(rapidxml::xml_node<>* XMLNode);
		void processEnvironment(rapidxml::xml_node<>* XMLNode);
		void processLight(rapidxml::xml_node<>* XMLNode, SceneNode *pParent = 0);
		void processCamera(rapidxml::xml_node<>* XMLNode, SceneNode *pParent = 0);

		void processLevels(rapidxml::xml_node<>* XMLRoot, std::vector<LevelInformation> &levels);
		LevelInformation processLevel(rapidxml::xml_node<>* XMLNode);
		
		void processActors(rapidxml::xml_node<>* XMLRoot, std::map<String, Actor*> &actors, std::vector<CompetitorMarble*> &friends, std::vector<CompetitorMarble*> &rivals, std::vector<CollectibleMarble*> &targets, CompetitorMarble **focus);
		void processActor(rapidxml::xml_node<>* XMLNode, std::map<String, Actor*> &actors, std::vector<CompetitorMarble*> &friends, std::vector<CompetitorMarble*> &rivals, std::vector<CollectibleMarble*> &targets, CompetitorMarble **focus);
		void processBody(rapidxml::xml_node<>* XMLNode, SceneNode *pParent, OgreBulletCollisions::CollisionShape *pCollisionShape, OgreBulletDynamics::RigidBody **pBody);
		void processBehavior(rapidxml::xml_node<>* XMLNode, Actor *pActor, std::map<String, Actor*> &actors, IBehavior **pBehavior);
		OgreBulletCollisions::CollisionShape* processCollisionShape(rapidxml::xml_node<>* XMLNode, Ogre::Entity* entity);
		OgreBulletCollisions::CollisionShape* processSphereCollisionShape(rapidxml::xml_node<>* XMLNode);
		OgreBulletCollisions::CollisionShape* processBoxCollisionShape(rapidxml::xml_node<>* XMLNode);
		OgreBulletCollisions::CollisionShape* processAutoSphereCollisionShape(rapidxml::xml_node<>* XMLNode, Ogre::Entity* entity);
		OgreBulletCollisions::CollisionShape* processAutoBoxCollisionShape(rapidxml::xml_node<>* XMLNode, Ogre::Entity* entity);
		OgreBulletCollisions::CollisionShape* processAutoCylinderCollisionShape(rapidxml::xml_node<>* XMLNode, Ogre::Entity* entity);
		OgreBulletCollisions::CollisionShape* processTriangleMeshCollisionShape(rapidxml::xml_node<>* XMLNode, Ogre::Entity* entity);
		OgreBulletCollisions::CollisionShape* processConvexHullCollisionShape(rapidxml::xml_node<>* XMLNode, Ogre::Entity* entity);
		OgreBulletCollisions::CollisionShape* processConvexHullCollisionDecompositionShape(rapidxml::xml_node<>* XMLNode, Ogre::Entity* entity);
		
		void processNode(rapidxml::xml_node<>* XMLNode, SceneNode *pParent = 0);
		void processEntity(rapidxml::xml_node<>* XMLNode, SceneNode *pParent);
		void processParticleSystem(rapidxml::xml_node<>* XMLNode, SceneNode *pParent);
		void processFog(rapidxml::xml_node<>* XMLNode);
		void processSkyBox(rapidxml::xml_node<>* XMLNode);
		void processSkyDome(rapidxml::xml_node<>* XMLNode);
		void processSkyPlane(rapidxml::xml_node<>* XMLNode);
		void processClipping(rapidxml::xml_node<>* XMLNode);
		void processLightRange(rapidxml::xml_node<>* XMLNode, Light *pLight);
		void processLightAttenuation(rapidxml::xml_node<>* XMLNode, Light *pLight);

		String getAttrib(rapidxml::xml_node<>* XMLNode, const String &parameter, const String &defaultValue = "");
		Real getAttribReal(rapidxml::xml_node<>* XMLNode, const String &parameter, Real defaultValue = 0);
		bool getAttribBool(rapidxml::xml_node<>* XMLNode, const String &parameter, bool defaultValue = false);

		Vector3 parseVector3(rapidxml::xml_node<>* XMLNode);
		Quaternion parseQuaternion(rapidxml::xml_node<>* XMLNode);
		ColourValue parseColour(rapidxml::xml_node<>* XMLNode);

		SceneManager *mSceneMgr;
		SceneNode *mAttachNode;
		String m_sGroupName;
		String m_sPrependNode;
		lua_State* mLuaState;

		OgreBulletDynamics::DynamicsWorld *mWorld;
	};
}

#endif // DOT_LEVELLOADER_H
