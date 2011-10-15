#include "DotLevelLoader.h"
#include <Ogre.h>

#include "rapidxml_utils.hpp"

#include "Actor.h"
#include "CollectibleMarble.h"
#include "CompetitorMarble.h"
#include "StaticActor.h"
#include "HealthPowerUp.h"
#include "FloatPowerUp.h"
#include "StickyPowerUp.h"
#include "SpeedPowerUp.h"
#include "BouncePowerUp.h"
#include "HelpPowerUp.h"
#include "TeleportPowerUp.h"

#include "PlayerBehavior.h"
#include "LostBehavior.h"
#include "InputManager.h"
#include "RivalBehavior.h"

using namespace std;
using namespace Ogre;

void DotLevelLoader::parseDotLevel(const String &SceneName, const String &groupName, SceneManager *yourSceneMgr, SceneNode *pAttachNode, const String &sPrependNode)
{
    // set up shared object values
    m_sGroupName = groupName;
    mSceneMgr = yourSceneMgr;
    m_sPrependNode = sPrependNode;
    staticObjects.clear();
    dynamicObjects.clear();

    rapidxml::xml_document<> XMLDoc;    // character type defaults to char

    rapidxml::xml_node<>* XMLRoot;

    String fileName = SceneName;
    FileLocator * fileLocator = (FileLocator * )Ogre::ResourceGroupManager::getSingletonPtr();
    Ogre::Archive* fileArchive = fileLocator->Find(fileName);

    rapidxml::file<> file(fileName.c_str());

    // Open the .scene File
    XMLDoc.parse<0>( file.data() );

    // Grab the scene node
    XMLRoot = XMLDoc.first_node("scene");

    // Validate the File
    //if( String( XMLRoot->first_attribute()->value()) != "formatVersion"  ) {
    //   LogManager::getSingleton().logMessage( "[DotLevelLoader] Error: Invalid .scene File. Missing <scene>" );
    //   return;
    //}

    // figure out where to attach any nodes we create
    mAttachNode = pAttachNode;
    if(!mAttachNode)
        mAttachNode = mSceneMgr->getRootSceneNode();

    // Process the scene
    processScene(XMLRoot);
}

void DotLevelLoader::parseActors(const String &actorFileName, const String &groupName, SceneManager *yourSceneMgr, OgreBulletDynamics::DynamicsWorld *yourBulletWorld, std::map<String, Actor*> &actors, std::vector<CompetitorMarble*> &friends, std::vector<CompetitorMarble*> &rivals, std::vector<CollectibleMarble*> &targets, CompetitorMarble **focus, SceneNode *pAttachNode, const String &sPrependNode)
{
    // set up shared object values
    m_sGroupName = groupName;
    mSceneMgr = yourSceneMgr;
    m_sPrependNode = sPrependNode;
    mWorld = yourBulletWorld;

    rapidxml::xml_document<> XMLDoc;    // character type defaults to char

    rapidxml::xml_node<>* XMLRoot;

    String fileName = actorFileName;
    FileLocator * fileLocator = (FileLocator * )Ogre::ResourceGroupManager::getSingletonPtr();
    Ogre::Archive* fileArchive = fileLocator->Find(fileName);

    rapidxml::file<> file(fileName.c_str());

    // Open the .scene File
    XMLDoc.parse<0>( file.data() );

    // Grab the actors node
    XMLRoot = XMLDoc.first_node("actors");

    // Process the actors
    processActors(XMLRoot, actors, friends, rivals, targets, focus);
}

void DotLevelLoader::parseLevels(const String &FileName, std::vector<LevelInformation> &levels)
{
    rapidxml::xml_document<> XMLDoc;    // character type defaults to char

    rapidxml::xml_node<>* XMLRoot;

    String fileName = FileName;
    FileLocator * fileLocator = (FileLocator * )Ogre::ResourceGroupManager::getSingletonPtr();
    Ogre::Archive* fileArchive = fileLocator->Find(fileName);

    rapidxml::file<> file(fileName.c_str());

    // Open the .scene File
    XMLDoc.parse<0>( file.data() );

    // Grab the actors node
    XMLRoot = XMLDoc.first_node("levels");

    // Process the actors
    processLevels(XMLRoot, levels);
}

void DotLevelLoader::reloadNodesAndActors(const String &dotLevelFileName, const String &actorFileName, const std::vector<std::string> &listToRestore, const String &groupName, SceneManager *yourSceneMgr, OgreBulletDynamics::DynamicsWorld *yourBulletWorld, std::map<String, Actor*> &actors, std::vector<CompetitorMarble*> &friends, std::vector<CompetitorMarble*> &rivals, std::vector<CollectibleMarble*> &targets, CompetitorMarble **focus)
{
    // set up shared object values
    m_sGroupName = groupName;
    mSceneMgr = yourSceneMgr;
    mWorld = yourBulletWorld;

    mAttachNode = mSceneMgr->getRootSceneNode();

    rapidxml::xml_document<> XMLDocDotLevel;
    rapidxml::xml_document<> XMLDocActors;

    FileLocator * fileLocator = (FileLocator * )Ogre::ResourceGroupManager::getSingletonPtr();
    String filename = actorFileName;
    Ogre::Archive* fileArchive = fileLocator->Find(filename);
    rapidxml::file<> actorfile(filename.c_str());
    filename = dotLevelFileName;
    fileArchive = fileLocator->Find(filename);
    rapidxml::file<> dotlevelfile(filename.c_str());

    XMLDocDotLevel.parse<0>( dotlevelfile.data() );
    XMLDocActors.parse<0>( actorfile.data() );

    rapidxml::xml_node<>* pElement;
    pElement = XMLDocDotLevel.first_node("scene")->first_node("nodes")->first_node("node");
    while(pElement) {
        if(std::find(listToRestore.begin(),listToRestore.end(),getAttrib(pElement,"name",""))!=listToRestore.end())
            processEntity(pElement->first_node("entity"),mSceneMgr->getSceneNode(getAttrib(pElement,"name","")));
        pElement = pElement->next_sibling("node");
    }

    pElement = XMLDocActors.first_node("actors")->first_node("actor");
    while(pElement) {
        if(std::find(listToRestore.begin(),listToRestore.end(),getAttrib(pElement,"name",""))!=listToRestore.end())
            processActor(pElement, actors, friends, rivals, targets, focus);
        pElement = pElement->next_sibling("actor");
    }

}

void DotLevelLoader::processLevels(rapidxml::xml_node<>* XMLRoot, std::vector<LevelInformation> &levels)
{
    rapidxml::xml_node<>* pElement;

    pElement = XMLRoot->first_node("level");
    while(pElement) {
        LevelInformation li = processLevel(pElement);
        std::vector<LevelInformation>::iterator i;
        for(i = levels.begin(); i != levels.end() && i->number < li.number; i++);
        levels.insert(i, li);
        pElement = pElement->next_sibling("level");
    }
}

LevelInformation DotLevelLoader::processLevel(rapidxml::xml_node<>* XMLNode)
{
    // Construct the node's name
    String file = getAttrib(XMLNode, "file");
    int number = getAttribReal(XMLNode, "number");
    String name = getAttrib(XMLNode, "name", "Level     " + Ogre::StringConverter::toString(number));
    String music = getAttrib(XMLNode, "music");

    LevelInformation li;
    li.levelname = name;
    li.filename = file;
    li.number = number;
    li.music = music;
    return li;
}


void DotLevelLoader::processActors(rapidxml::xml_node<>* XMLRoot, std::map<String, Actor*> &actors, std::vector<CompetitorMarble*> &friends, std::vector<CompetitorMarble*> &rivals, std::vector<CollectibleMarble*> &targets, CompetitorMarble **focus)
{
    rapidxml::xml_node<>* pElement;

    pElement = XMLRoot->first_node("actor");
    while(pElement) {
        processActor(pElement, actors, friends, rivals, targets, focus);
        pElement = pElement->next_sibling("actor");
    }
}

void DotLevelLoader::processActor(rapidxml::xml_node<>* XMLNode, std::map<String, Actor*> &actors, std::vector<CompetitorMarble*> &friends, std::vector<CompetitorMarble*> &rivals, std::vector<CollectibleMarble*> &targets, CompetitorMarble **focus)
{
    // Construct the node's name
    String name = getAttrib(XMLNode, "name");
    String actorType = getAttrib(XMLNode, "type");
    String team = getAttrib(XMLNode, "team");
    String focusString = getAttrib(XMLNode, "focus");
    String entityName = getAttrib(XMLNode, "entity");
    Real maxSpeed = getAttribReal(XMLNode, "maxSpeed", -1);
    int duration = getAttribReal(XMLNode, "duration", 30);
    String layoutFile = getAttrib(XMLNode, "layout");
    String target = getAttrib(XMLNode, "target");

    rapidxml::xml_node<>* pElement;

    // Process collisionshape (*)
    pElement = XMLNode->first_node("entity");
    OgreBulletCollisions::CollisionShape *collisionShape;
    Ogre::Entity *ent = mSceneMgr->getEntity(entityName);
    pElement = XMLNode->first_node("collisionShape");
    if(pElement)
        collisionShape = processCollisionShape(pElement, ent);

    // Process body (*)
    pElement = XMLNode->first_node("body");
    OgreBulletDynamics::RigidBody *body;
    if(pElement) {
        processBody(pElement,ent->getParentSceneNode(),collisionShape,&body);
    }

    Actor *actor = 0;
    if(!actorType.empty()) {
        if(actorType.compare("CollectibleMarble")==0) {
            CollectibleMarble* cm = new CollectibleMarble(ent, body);
            actors[name] = actor = cm;
            if(team.compare("target")==0)
                targets.push_back(cm);
        } else if(actorType.compare("CompetitorMarble")==0) {
            CompetitorMarble* cm = new CompetitorMarble(ent, body);
            actors[name] = actor = cm;
            if(team.compare("friend")==0)
                friends.push_back(cm);
            else if(team.compare("rival")==0)
                rivals.push_back(cm);
            if(focusString.compare("true")==0)
                *focus = cm;
        } else if(actorType.compare("StaticActor")==0) {
            actors[name] = actor = new StaticActor(ent, body);
        } else if(actorType.compare("HealthPowerUp")==0) {
            actors[name] = actor = new HealthPowerUp(ent, body);
        } else if(actorType.compare("FloatPowerUp")==0) {
            actors[name] = actor = new FloatPowerUp(ent, body, duration);
        } else if(actorType.compare("StickyPowerUp")==0) {
            actors[name] = actor = new StickyPowerUp(ent, body, duration);
        } else if(actorType.compare("SpeedPowerUp")==0) {
            actors[name] = actor = new SpeedPowerUp(ent, body, duration);
        } else if(actorType.compare("BouncePowerUp")==0) {
            actors[name] = actor = new BouncePowerUp(ent, body, duration);
        } else if(actorType.compare("TeleportPowerUp")==0) {
            actors[name] = actor = new TeleportPowerUp(ent, body, target);
        } else if(actorType.compare("HelpPowerUp")==0) {
            actors[name] = actor = new HelpPowerUp(ent, body, layoutFile);
        }

        if(actor && maxSpeed >= 0) {
            actor->setMaxSpeed(maxSpeed);
        }
    }

    pElement = XMLNode->first_node("behavior");
    IBehavior *behavior;
    if(pElement) {
        processBehavior(pElement,actor,actors,&behavior);
        actor->addBehavior(behavior);
    }
}


void DotLevelLoader::processScene(rapidxml::xml_node<>* XMLRoot)
{
    // Process the scene parameters
    String version = getAttrib(XMLRoot, "formatVersion", "unknown");

    String message = "[DotLevelLoader] Parsing dotScene file with version " + version;
    if(XMLRoot->first_attribute("ID"))
        message += ", id " + String(XMLRoot->first_attribute("ID")->value());
    if(XMLRoot->first_attribute("sceneManager"))
        message += ", scene manager " + String(XMLRoot->first_attribute("sceneManager")->value());
    if(XMLRoot->first_attribute("minOgreVersion"))
        message += ", min. Ogre version " + String(XMLRoot->first_attribute("minOgreVersion")->value());
    if(XMLRoot->first_attribute("author"))
        message += ", author " + String(XMLRoot->first_attribute("author")->value());

    LogManager::getSingleton().logMessage(message);

    rapidxml::xml_node<>* pElement;

    // Process nodes (?)
    pElement = XMLRoot->first_node("nodes");
    if(pElement)
        processNodes(pElement);

    // Process environment (?)
    pElement = XMLRoot->first_node("environment");
    if(pElement)
        processEnvironment(pElement);

    // Process light (?)
    pElement = XMLRoot->first_node("light");
    if(pElement)
        processLight(pElement);

    // Process camera (?)
    pElement = XMLRoot->first_node("camera");
    if(pElement)
        processCamera(pElement);
}

void DotLevelLoader::processNodes(rapidxml::xml_node<>* XMLNode)
{
    rapidxml::xml_node<>* pElement;

    // Process node (*)
    pElement = XMLNode->first_node("node");
    while(pElement) {
        processNode(pElement);
        pElement = pElement->next_sibling("node");
    }

    // Process position (?)
    pElement = XMLNode->first_node("position");
    if(pElement) {
        mAttachNode->setPosition(parseVector3(pElement));
        mAttachNode->setInitialState();
    }

    // Process rotation (?)
    pElement = XMLNode->first_node("rotation");
    if(pElement) {
        mAttachNode->setOrientation(parseQuaternion(pElement));
        mAttachNode->setInitialState();
    }

    // Process scale (?)
    pElement = XMLNode->first_node("scale");
    if(pElement) {
        mAttachNode->setScale(parseVector3(pElement));
        mAttachNode->setInitialState();
    }
}

void DotLevelLoader::processEnvironment(rapidxml::xml_node<>* XMLNode)
{
    rapidxml::xml_node<>* pElement;

    // Process fog (?)
    pElement = XMLNode->first_node("fog");
    if(pElement)
        processFog(pElement);

    // Process skyBox (?)
    pElement = XMLNode->first_node("skyBox");
    if(pElement)
        processSkyBox(pElement);

    // Process skyDome (?)
    pElement = XMLNode->first_node("skyDome");
    if(pElement)
        processSkyDome(pElement);

    // Process skyPlane (?)
    pElement = XMLNode->first_node("skyPlane");
    if(pElement)
        processSkyPlane(pElement);

    // Process clipping (?)
    pElement = XMLNode->first_node("clipping");
    if(pElement)
        processClipping(pElement);

    // Process colourAmbient (?)
    pElement = XMLNode->first_node("colourAmbient");
    if(pElement)
        mSceneMgr->setAmbientLight(parseColour(pElement));
}

void DotLevelLoader::processLight(rapidxml::xml_node<>* XMLNode, SceneNode *pParent)
{
    // Process attributes
    String name = getAttrib(XMLNode, "name");
    String id = getAttrib(XMLNode, "id");

    // Create the light
    Light *pLight = mSceneMgr->createLight(name);
    if(pParent)
        pParent->attachObject(pLight);

    String sValue = getAttrib(XMLNode, "type");
    if(sValue == "point")
        pLight->setType(Light::LT_POINT);
    else if(sValue == "directional")
        pLight->setType(Light::LT_DIRECTIONAL);
    else if(sValue == "spot")
        pLight->setType(Light::LT_SPOTLIGHT);
    else if(sValue == "radPoint")
        pLight->setType(Light::LT_POINT);

    pLight->setVisible(getAttribBool(XMLNode, "visible", true));
    pLight->setCastShadows(getAttribBool(XMLNode, "castShadows", true));

    rapidxml::xml_node<>* pElement;

    // Process position (?)
    pElement = XMLNode->first_node("position");
    if(pElement)
        pLight->setPosition(parseVector3(pElement));

    // Process normal (?)
    pElement = XMLNode->first_node("normal");
    if(pElement)
        pLight->setDirection(parseVector3(pElement));

    // Process colourDiffuse (?)
    pElement = XMLNode->first_node("colourDiffuse");
    if(pElement)
        pLight->setDiffuseColour(parseColour(pElement));

    // Process colourSpecular (?)
    pElement = XMLNode->first_node("colourSpecular");
    if(pElement)
        pLight->setSpecularColour(parseColour(pElement));

    if(sValue != "directional") {
        // Process lightRange (?)
        pElement = XMLNode->first_node("lightRange");
        if(pElement)
            processLightRange(pElement, pLight);

        // Process lightAttenuation (?)
        pElement = XMLNode->first_node("lightAttenuation");
        if(pElement)
            processLightAttenuation(pElement, pLight);
    }
}

void DotLevelLoader::processCamera(rapidxml::xml_node<>* XMLNode, SceneNode *pParent)
{
    // Process attributes
    String name = getAttrib(XMLNode, "name");
    String id = getAttrib(XMLNode, "id");
    Real fov = getAttribReal(XMLNode, "fov", 45);
    Real aspectRatio = getAttribReal(XMLNode, "aspectRatio", 1.3333);
    String projectionType = getAttrib(XMLNode, "projectionType", "perspective");

    // Create the camera
    Camera *pCamera = mSceneMgr->createCamera(name);
    Ogre::LogManager::getSingleton().logMessage(Ogre::String("[DotLevelLoader] Created camera with the name of ") + name);
    if(pParent)
        pParent->attachObject(pCamera);

    // Set the field-of-view
    //! @todo Is this always in degrees?
    pCamera->setFOVy(Ogre::Degree(fov));

    // Set the aspect ratio
    pCamera->setAspectRatio(aspectRatio);

    // Set the projection type
    if(projectionType == "perspective")
        pCamera->setProjectionType(PT_PERSPECTIVE);
    else if(projectionType == "orthographic")
        pCamera->setProjectionType(PT_ORTHOGRAPHIC);


    rapidxml::xml_node<>* pElement;

    // Process clipping (?)
    pElement = XMLNode->first_node("clipping");
    if(pElement) {
        Real nearDist = getAttribReal(pElement, "near", 1.0);
        pCamera->setNearClipDistance(nearDist);

        Real farDist =  getAttribReal(pElement, "far", 99999.0);
        pCamera->setFarClipDistance(farDist);
    }

    // Process position (?)
    pElement = XMLNode->first_node("position");
    if(pElement)
        pCamera->setPosition(parseVector3(pElement));

    // Process rotation (?)
    pElement = XMLNode->first_node("rotation");
    if(pElement)
        pCamera->setOrientation(parseQuaternion(pElement));
}

void DotLevelLoader::processNode(rapidxml::xml_node<>* XMLNode, SceneNode *pParent)
{
    // Construct the node's name
    String name = m_sPrependNode + getAttrib(XMLNode, "name");

    // Create the scene node
    SceneNode *pNode;
    if(name.empty()) {
        // Let Ogre choose the name
        if(pParent)
            pNode = pParent->createChildSceneNode();
        else
            pNode = mAttachNode->createChildSceneNode();
    } else {
        // Provide the name
        if(pParent)
            pNode = pParent->createChildSceneNode(name);
        else
            pNode = mAttachNode->createChildSceneNode(name);
    }

    // Process other attributes
    String id = getAttrib(XMLNode, "id");
    bool isTarget = getAttribBool(XMLNode, "isTarget");

    rapidxml::xml_node<>* pElement;

    // Process position (?)
    pElement = XMLNode->first_node("position");
    if(pElement) {
        pNode->setPosition(parseVector3(pElement));
        pNode->setInitialState();
    }

    // Process rotation (?)
    pElement = XMLNode->first_node("rotation");
    if(pElement) {
        pNode->setOrientation(parseQuaternion(pElement));
        pNode->setInitialState();
    }

    // Process scale (?)
    pElement = XMLNode->first_node("scale");
    if(pElement) {
        pNode->setScale(parseVector3(pElement));
        pNode->setInitialState();
    }

    // Process node (*)
    pElement = XMLNode->first_node("node");
    while(pElement) {
        processNode(pElement, pNode);
        pElement = pElement->next_sibling("node");
    }

    // Process entity (*)
    pElement = XMLNode->first_node("entity");
    while(pElement) {
        processEntity(pElement, pNode);
        pElement = pElement->next_sibling("entity");
    }

    // Process light (*)
    pElement = XMLNode->first_node("light");
    while(pElement) {
        processLight(pElement, pNode);
        pElement = pElement->next_sibling("light");
    }

    // Process camera (*)
    pElement = XMLNode->first_node("camera");
    while(pElement) {
        processCamera(pElement, pNode);
        pElement = pElement->next_sibling("camera");
    }

    // Process particleSystem (*)
    pElement = XMLNode->first_node("particleSystem");
    while(pElement) {
        processParticleSystem(pElement, pNode);
        pElement = pElement->next_sibling("particleSystem");
    }
}

void DotLevelLoader::processEntity(rapidxml::xml_node<>* XMLNode, SceneNode *pParent)
{
    // Process attributes
    String name = getAttrib(XMLNode, "name");
    String id = getAttrib(XMLNode, "id");
    String meshFile = getAttrib(XMLNode, "meshFile");
    String materialFile = getAttrib(XMLNode, "materialFile");
    bool isStatic = getAttribBool(XMLNode, "static", false);;
    bool castShadows = getAttribBool(XMLNode, "castShadows", true);

    // TEMP: Maintain a list of static and dynamic objects
    if(isStatic)
        staticObjects.push_back(name);
    else
        dynamicObjects.push_back(name);

    // Create the entity
    Entity *ent = 0;
    try {
        MeshManager::getSingleton().load(meshFile, m_sGroupName);
        ent = mSceneMgr->createEntity(name, meshFile);
        ent->setCastShadows(castShadows);
        pParent->attachObject(ent);

        if(!materialFile.empty())
            ent->setMaterialName(materialFile);
    } catch(Ogre::Exception &/*e*/) {
        LogManager::getSingleton().logMessage("[DotLevelLoader] Error loading an entity!");
    }
}

OgreBulletCollisions::CollisionShape* DotLevelLoader::processCollisionShape(rapidxml::xml_node<>* XMLNode, Ogre::Entity* entity)
{
    rapidxml::xml_node<>* pElement;
    OgreBulletCollisions::CollisionShape *collisionShape;
    pElement = XMLNode->first_node("sphereCollisionShape");
    if(pElement)
        collisionShape = processSphereCollisionShape(pElement);
    pElement = XMLNode->first_node("boxCollisionShape");
    if(pElement)
        collisionShape = processBoxCollisionShape(pElement);
    pElement = XMLNode->first_node("autoSphereCollisionShape");
    if(pElement) {
        collisionShape = processAutoSphereCollisionShape(pElement, entity);
        Vector3 scale = entity->getParentSceneNode()->getScale();
        collisionShape->getBulletShape()->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
    }
    pElement = XMLNode->first_node("autoBoxCollisionShape");
    if(pElement) {
        collisionShape = processAutoBoxCollisionShape(pElement, entity);
        Vector3 scale = entity->getParentSceneNode()->getScale();
        collisionShape->getBulletShape()->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
    }
    pElement = XMLNode->first_node("autoCylinderCollisionShape");
    if(pElement) {
        collisionShape = processAutoCylinderCollisionShape(pElement, entity);
        Vector3 scale = entity->getParentSceneNode()->getScale();
        collisionShape->getBulletShape()->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
    }
    pElement = XMLNode->first_node("triangleMeshCollisionShape");
    if(pElement) {
        collisionShape = processTriangleMeshCollisionShape(pElement, entity);
        Vector3 scale = entity->getParentSceneNode()->getScale();
        collisionShape->getBulletShape()->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
    }
    pElement = XMLNode->first_node("convexHullCollisionShape");
    if(pElement) {
        collisionShape = processConvexHullCollisionShape(pElement, entity);
        Vector3 scale = entity->getParentSceneNode()->getScale();
        collisionShape->getBulletShape()->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
    }
    pElement = XMLNode->first_node("convexHullDecompositionCollisionShape");
    if(pElement) {
        collisionShape = processConvexHullCollisionDecompositionShape(pElement, entity);
        Vector3 scale = entity->getParentSceneNode()->getScale();
        collisionShape->getBulletShape()->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
    }
    return collisionShape;
}

OgreBulletCollisions::CollisionShape* DotLevelLoader::processSphereCollisionShape(rapidxml::xml_node<>* XMLNode)
{
    Real radius = getAttribReal(XMLNode, "radius", 1);
    OgreBulletCollisions::SphereCollisionShape *sphereShape = new OgreBulletCollisions::SphereCollisionShape(radius);
    return sphereShape;
}

OgreBulletCollisions::CollisionShape* DotLevelLoader::processBoxCollisionShape(rapidxml::xml_node<>* XMLNode)
{
    Real sidex = getAttribReal(XMLNode, "sidex", 1);
    Real sidey = getAttribReal(XMLNode, "sidey", 1);
    Real sidez = getAttribReal(XMLNode, "sidez", 1);
    OgreBulletCollisions::BoxCollisionShape *boxShape = new OgreBulletCollisions::BoxCollisionShape(Vector3(sidex, sidey, sidez));
    return boxShape;
}

OgreBulletCollisions::CollisionShape* DotLevelLoader::processAutoSphereCollisionShape(rapidxml::xml_node<>* XMLNode, Ogre::Entity* entity)
{
    if(entity) {
        OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter;
        OgreBulletCollisions::SphereCollisionShape *sceneTriMeshShape;
        trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(entity);
        sceneTriMeshShape = trimeshConverter->createSphere();
        delete trimeshConverter;
        return sceneTriMeshShape;
    }
    return 0;
}

OgreBulletCollisions::CollisionShape* DotLevelLoader::processAutoBoxCollisionShape(rapidxml::xml_node<>* XMLNode, Ogre::Entity* entity)
{
    if(entity) {
        OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter;
        OgreBulletCollisions::BoxCollisionShape *sceneTriMeshShape;
        trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(entity);
        sceneTriMeshShape = trimeshConverter->createBox();
        delete trimeshConverter;
        return sceneTriMeshShape;
    }
    return 0;
}

OgreBulletCollisions::CollisionShape* DotLevelLoader::processAutoCylinderCollisionShape(rapidxml::xml_node<>* XMLNode, Ogre::Entity* entity)
{
    if(entity) {
        OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter;
        OgreBulletCollisions::CylinderCollisionShape *sceneTriMeshShape;
        trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(entity);
        sceneTriMeshShape = trimeshConverter->createCylinder();
        delete trimeshConverter;
        return sceneTriMeshShape;
    }
    return 0;
}

OgreBulletCollisions::CollisionShape* DotLevelLoader::processTriangleMeshCollisionShape(rapidxml::xml_node<>* XMLNode, Ogre::Entity* entity)
{
    if(entity) {
        OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter;
        OgreBulletCollisions::TriangleMeshCollisionShape *sceneTriMeshShape;
        trimeshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(entity);
        sceneTriMeshShape = trimeshConverter->createTrimesh();
        delete trimeshConverter;
        return sceneTriMeshShape;
    }
    return 0;
}

OgreBulletCollisions::CollisionShape* DotLevelLoader::processConvexHullCollisionShape(rapidxml::xml_node<>* XMLNode, Ogre::Entity* entity)
{
    OgreBulletCollisions::StaticMeshToShapeConverter *meshConverter;
    OgreBulletCollisions::ConvexHullCollisionShape *convexHullShape;
    if(entity) {
        meshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(entity);
        convexHullShape = meshConverter->createConvex();
        delete meshConverter;
        return convexHullShape;
    }
    return 0;
}

OgreBulletCollisions::CollisionShape* DotLevelLoader::processConvexHullCollisionDecompositionShape(rapidxml::xml_node<>* XMLNode, Ogre::Entity* entity)
{
    int depth = getAttribReal(XMLNode, "depth", 5);
    Real cpercent = getAttribReal(XMLNode, "cpercent", 5);
    Real ppercent = getAttribReal(XMLNode, "ppercent", 15);
    int maxv = getAttribReal(XMLNode, "maxv", 32);
    Real skinwidth = getAttribReal(XMLNode, "skinwidth", 0);
    OgreBulletCollisions::StaticMeshToShapeConverter *meshConverter;
    OgreBulletCollisions::CompoundCollisionShape *convexHullShape;
    if(entity) {
        meshConverter = new OgreBulletCollisions::StaticMeshToShapeConverter(entity);
        convexHullShape = meshConverter->createConvexDecomposition(depth, cpercent, ppercent, maxv, skinwidth);
        delete meshConverter;
        return convexHullShape;
    }
    return 0;
}

void DotLevelLoader::processBehavior(rapidxml::xml_node<>* XMLNode, Actor *pActor, std::map<String, Actor*> &actors, IBehavior **pBehavior)
{
    String type = getAttrib(XMLNode, "type");

    if(type.compare("PlayerBehavior")==0) {
        String node = getAttrib(XMLNode, "camNode");
        *pBehavior = new PlayerBehavior(pActor, mSceneMgr->getSceneNode(node), InputManager::getSingleton().getKeyboard(), InputManager::getSingleton().getMouse());
    } else if(type.compare("RivalBehavior")==0) {
        String follow = getAttrib(XMLNode, "follow");
        *pBehavior = new RivalBehavior(pActor, follow, mLuaState);
    } else if(type.compare("LostBehavior")==0) {
        *pBehavior = new LostBehavior(pActor);
    }
}

void DotLevelLoader::processBody(rapidxml::xml_node<>* XMLNode, SceneNode *pParent, OgreBulletCollisions::CollisionShape *pCollisionShape, OgreBulletDynamics::RigidBody **pBody)
{
    String name = getAttrib(XMLNode, "name");
    Real restitution = getAttribReal(XMLNode, "restitution", 1);
    Real friction = getAttribReal(XMLNode, "friction", 1);
    Real mass = getAttribReal(XMLNode, "mass", 1);
    Real linearDamping = getAttribReal(XMLNode, "linearDamping", 0);
    Real angularDamping = getAttribReal(XMLNode, "angularDamping", 0);
    Real ccdMotionThreshold = getAttribReal(XMLNode, "ccdMotionThreshold", 0);
    Real ccdSweptSphereRadius = getAttribReal(XMLNode, "ccdSweptSphereRadius", 0);
    bool staticAttribute = getAttribBool(XMLNode, "static", false);

    OgreBulletDynamics::RigidBody *body = new OgreBulletDynamics::RigidBody(name, mWorld);
    Vector3 pos = pParent->getPosition();
    Quaternion quat = pParent->getOrientation();
    if(staticAttribute)
        body->setStaticShape(pParent, pCollisionShape, restitution, friction, pos, quat);
    else
        body->setShape(pParent, pCollisionShape, restitution, friction, mass, pos, quat);
    body->setDamping(linearDamping, angularDamping);
    body->getBulletRigidBody()->setCcdMotionThreshold(ccdMotionThreshold);
    body->getBulletRigidBody()->setCcdSweptSphereRadius(ccdSweptSphereRadius);

    *pBody = body;
}

void DotLevelLoader::processParticleSystem(rapidxml::xml_node<>* XMLNode, SceneNode *pParent)
{
    // Process attributes
    String name = getAttrib(XMLNode, "name");
    String id = getAttrib(XMLNode, "id");
    String file = getAttrib(XMLNode, "file");

    // Create the particle system
    try {
        ParticleSystem *pParticles = mSceneMgr->createParticleSystem(name, file);
        pParent->attachObject(pParticles);
    } catch(Ogre::Exception &/*e*/) {
        LogManager::getSingleton().logMessage("[DotLevelLoader] Error creating a particle system!");
    }
}

void DotLevelLoader::processFog(rapidxml::xml_node<>* XMLNode)
{
    // Process attributes
    Real expDensity = getAttribReal(XMLNode, "expDensity", 0.001);
    Real linearStart = getAttribReal(XMLNode, "linearStart", 0.0);
    Real linearEnd = getAttribReal(XMLNode, "linearEnd", 1.0);

    FogMode mode = FOG_NONE;
    String sMode = getAttrib(XMLNode, "mode");
    if(sMode == "none")
        mode = FOG_NONE;
    else if(sMode == "exp")
        mode = FOG_EXP;
    else if(sMode == "exp2")
        mode = FOG_EXP2;
    else if(sMode == "linear")
        mode = FOG_LINEAR;

    rapidxml::xml_node<>* pElement;

    // Process colourDiffuse (?)
    ColourValue colourDiffuse = ColourValue::White;
    pElement = XMLNode->first_node("colourDiffuse");
    if(pElement)
        colourDiffuse = parseColour(pElement);

    // Setup the fog
    mSceneMgr->setFog(mode, colourDiffuse, expDensity, linearStart, linearEnd);
}

void DotLevelLoader::processSkyBox(rapidxml::xml_node<>* XMLNode)
{
    // Process attributes
    Real distance = getAttribReal(XMLNode, "distance", 5000);
    String material = getAttrib(XMLNode, "material");
    bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);

    rapidxml::xml_node<>* pElement;

    // Process rotation (?)
    Quaternion rotation = Quaternion::IDENTITY;
    pElement = XMLNode->first_node("rotation");
    if(pElement)
        rotation = parseQuaternion(pElement);

    // Setup the sky box
    mSceneMgr->setSkyBox(true, material, distance, drawFirst, rotation, m_sGroupName);
}

void DotLevelLoader::processSkyDome(rapidxml::xml_node<>* XMLNode)
{
    // Process attributes
    String material = getAttrib(XMLNode, "material");
    Real curvature = getAttribReal(XMLNode, "curvature", 10);
    Real tiling = getAttribReal(XMLNode, "tiling", 8);
    Real distance = getAttribReal(XMLNode, "distance", 4000);
    bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);

    rapidxml::xml_node<>* pElement;

    // Process rotation (?)
    Quaternion rotation = Quaternion::IDENTITY;
    pElement = XMLNode->first_node("rotation");
    if(pElement)
        rotation = parseQuaternion(pElement);

    // Setup the sky dome
    mSceneMgr->setSkyDome(true, material, curvature, tiling, distance, drawFirst, rotation, 16, 16, -1, m_sGroupName);
}

void DotLevelLoader::processSkyPlane(rapidxml::xml_node<>* XMLNode)
{
    // Process attributes
    String material = getAttrib(XMLNode, "material");
    Real planeX = getAttribReal(XMLNode, "planeX", 0);
    Real planeY = getAttribReal(XMLNode, "planeY", -1);
    Real planeZ = getAttribReal(XMLNode, "planeX", 0);
    Real planeD = getAttribReal(XMLNode, "planeD", 5000);
    Real scale = getAttribReal(XMLNode, "scale", 1000);
    Real bow = getAttribReal(XMLNode, "bow", 0);
    Real tiling = getAttribReal(XMLNode, "tiling", 10);
    bool drawFirst = getAttribBool(XMLNode, "drawFirst", true);

    // Setup the sky plane
    Plane plane;
    plane.normal = Vector3(planeX, planeY, planeZ);
    plane.d = planeD;
    mSceneMgr->setSkyPlane(true, plane, material, scale, tiling, drawFirst, bow, 1, 1, m_sGroupName);
}

void DotLevelLoader::processClipping(rapidxml::xml_node<>* XMLNode)
{
    //! @todo Implement this

    // Process attributes
    Real fNear = getAttribReal(XMLNode, "near", 0);
    Real fFar = getAttribReal(XMLNode, "far", 1);
}

void DotLevelLoader::processLightRange(rapidxml::xml_node<>* XMLNode, Light *pLight)
{
    // Process attributes
    Real inner = getAttribReal(XMLNode, "inner");
    Real outer = getAttribReal(XMLNode, "outer");
    Real falloff = getAttribReal(XMLNode, "falloff", 1.0);

    // Setup the light range
    pLight->setSpotlightRange(Angle(inner), Angle(outer), falloff);
}

void DotLevelLoader::processLightAttenuation(rapidxml::xml_node<>* XMLNode, Light *pLight)
{
    // Process attributes
    Real range = getAttribReal(XMLNode, "range");
    Real constant = getAttribReal(XMLNode, "constant");
    Real linear = getAttribReal(XMLNode, "linear");
    Real quadratic = getAttribReal(XMLNode, "quadratic");

    // Setup the light attenuation
    pLight->setAttenuation(range, constant, linear, quadratic);
}


String DotLevelLoader::getAttrib(rapidxml::xml_node<>* XMLNode, const String &attrib, const String &defaultValue)
{
    if(XMLNode->first_attribute(attrib.c_str()))
        return XMLNode->first_attribute(attrib.c_str())->value();
    else
        return defaultValue;
}

Real DotLevelLoader::getAttribReal(rapidxml::xml_node<>* XMLNode, const String &attrib, Real defaultValue)
{
    if(XMLNode->first_attribute(attrib.c_str()))
        return StringConverter::parseReal(XMLNode->first_attribute(attrib.c_str())->value());
    else
        return defaultValue;
}

bool DotLevelLoader::getAttribBool(rapidxml::xml_node<>* XMLNode, const String &attrib, bool defaultValue)
{
    if(!XMLNode->first_attribute(attrib.c_str()))
        return defaultValue;

    if(String(XMLNode->first_attribute(attrib.c_str())->value()) == "true")
        return true;

    return false;
}

Vector3 DotLevelLoader::parseVector3(rapidxml::xml_node<>* XMLNode)
{
    return Vector3(
               StringConverter::parseReal(XMLNode->first_attribute("x")->value()),
               StringConverter::parseReal(XMLNode->first_attribute("y")->value()),
               StringConverter::parseReal(XMLNode->first_attribute("z")->value())
           );
}

Quaternion DotLevelLoader::parseQuaternion(rapidxml::xml_node<>* XMLNode)
{
    //! @todo Fix this crap!

    Quaternion orientation;

    if(XMLNode->first_attribute("qx")) {
        orientation.x = StringConverter::parseReal(XMLNode->first_attribute("qx")->value());
        orientation.y = StringConverter::parseReal(XMLNode->first_attribute("qy")->value());
        orientation.z = StringConverter::parseReal(XMLNode->first_attribute("qz")->value());
        orientation.w = StringConverter::parseReal(XMLNode->first_attribute("qw")->value());
    } else if(XMLNode->first_attribute("axisX")) {
        Vector3 axis;
        axis.x = StringConverter::parseReal(XMLNode->first_attribute("axisX")->value());
        axis.y = StringConverter::parseReal(XMLNode->first_attribute("axisY")->value());
        axis.z = StringConverter::parseReal(XMLNode->first_attribute("axisZ")->value());
        Real angle = StringConverter::parseReal(XMLNode->first_attribute("angle")->value());;
        orientation.FromAngleAxis(Ogre::Angle(angle), axis);
    } else if(XMLNode->first_attribute("angleX")) {
        Vector3 axis;
        axis.x = StringConverter::parseReal(XMLNode->first_attribute("angleX")->value());
        axis.y = StringConverter::parseReal(XMLNode->first_attribute("angleY")->value());
        axis.z = StringConverter::parseReal(XMLNode->first_attribute("angleZ")->value());
        //orientation.FromAxes(&axis);
        //orientation.F
    }

    return orientation;
}

ColourValue DotLevelLoader::parseColour(rapidxml::xml_node<>* XMLNode)
{
    return ColourValue(
               StringConverter::parseReal(XMLNode->first_attribute("r")->value()),
               StringConverter::parseReal(XMLNode->first_attribute("g")->value()),
               StringConverter::parseReal(XMLNode->first_attribute("b")->value()),
               XMLNode->first_attribute("a") != NULL ? StringConverter::parseReal(XMLNode->first_attribute("a")->value()) : 1
           );
}

String DotLevelLoader::getProperty(const String &ndNm, const String &prop)
{
    for ( unsigned int i = 0 ; i < nodeProperties.size(); i++ ) {
        if ( nodeProperties[i].nodeName == ndNm && nodeProperties[i].propertyNm == prop ) {
            return nodeProperties[i].valueName;
        }
    }

    return "";
}
