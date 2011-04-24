#include "RenderModel.h"
#include <OgreMatrix4.h>

using namespace Ogre;

// ========================================================================
// RenderObject Implementation
// ========================================================================
RenderObject::RenderObject(PhysicsObject * object, SceneManager * mgr)
	: mp_object(object), mp_mgr(mgr)
{
}

SceneManager * RenderObject::getSceneManager()
{
	return mp_mgr;
}

PhysicsObject * RenderObject::getObject() {
	return mp_object;
}

bool RenderObject::operator==(const RenderObject &other) const
{
	return (mp_object == other.mp_object);
}


// ========================================================================
// ShipRO Implementation
// ========================================================================
ShipRO::ShipRO(PhysicsObject * object, SceneManager * mgr)
	: RenderObject(object, mgr), mp_shipNode(NULL), mp_shipRotateNode(NULL), mp_shipEntity(NULL),
	mp_spotLight(NULL), mp_pointLight(NULL)
{
}

/** Updates the node based on passed time and camera orientation (useful for sprites) */
void ShipRO::updateNode(Real elapsedTime, Quaternion camOrientation)
{
	mp_shipNode->setPosition(getObject()->getPosition());
	mp_shipNode->setOrientation(getObject()->getOrientation());
}

void ShipRO::loadSceneResources() 
{
}

void ShipRO::buildNode(int entityIndex)
{
	mp_shipNode = getSceneManager()->getRootSceneNode()->createChildSceneNode();
	std::stringstream oss;
	oss << "Ship" << entityIndex;
	mp_shipEntity = getSceneManager()->createEntity(oss.str(), "RZR-002.mesh");
	mp_shipEntity->setCastShadows(true);
	mp_shipNode->setScale(10, 10, 10);
	mp_shipRotateNode = mp_shipNode->createChildSceneNode();
	mp_shipRotateNode->setDirection(Vector3(0, 0, 1));
	mp_shipRotateNode->attachObject(mp_shipEntity);

	// Add a spot light to the ship of it's the player ship
	oss << "L";
	mp_spotLight = getSceneManager()->createLight(oss.str());
	mp_spotLight->setType(Light::LT_SPOTLIGHT);
	mp_spotLight->setDiffuseColour(0.8, 0.8, 1.0);
	mp_spotLight->setSpecularColour(0.2, 0.2, 1.0);
	mp_spotLight->setDirection(0, 0, -1);
	mp_spotLight->setPosition(Vector3(0, 30, 0));
	mp_spotLight->setSpotlightRange(Degree(20), Degree(45));
	mp_shipNode->attachObject(mp_spotLight);

	// Add a point light above the ship
	oss << "L";
	mp_pointLight = getSceneManager()->createLight(oss.str());
	mp_pointLight->setType(Ogre::Light::LT_POINT);
	mp_pointLight->setPosition(Ogre::Vector3(0, 30, 0));
	if (getObject()->getType() == ObjectType::SHIP) {
		mp_pointLight->setDiffuseColour(0.4, 0.1, 0.1);
		mp_pointLight->setSpecularColour(0.4, 0.4, 0.4);
	} else {
		mp_pointLight->setDiffuseColour(0.1, 0.1, 0.5);
		mp_pointLight->setSpecularColour(0.4, 0.4, 0.4);
	}
	mp_shipNode->attachObject(mp_pointLight);
}

void ShipRO::destroyNode()
{
	mp_shipNode->removeAllChildren();
	mp_shipNode->detachAllObjects();
	mp_shipRotateNode->removeAllChildren();
	mp_shipRotateNode->detachAllObjects();
	getSceneManager()->destroyMovableObject(mp_shipEntity);

	getSceneManager()->destroyLight(mp_spotLight);
	getSceneManager()->destroyLight(mp_pointLight);
}


// ========================================================================
// NpcShipRO Implementation
// ========================================================================
bool NpcShipRO::m_resourcesLoaded = false;

NpcShipRO::NpcShipRO(PhysicsObject * object, SceneManager * mgr)
	: ShipRO(object, mgr), mp_frameNode(NULL), mp_frameSprite(NULL)
{
}

void NpcShipRO::updateNode(Real elapsedTime, Quaternion camOrientation)
{
	ShipRO::updateNode(elapsedTime, camOrientation);
	mp_frameNode->setPosition(getObject()->getPosition());
	mp_frameNode->setOrientation(camOrientation);
}


void NpcShipRO::loadSceneResources()
{
	if(!m_resourcesLoaded) {
		Plane planeTarget = Plane(Ogre::Vector3::UNIT_Z, 0);
		MeshManager::getSingleton().createPlane("targetFrameSprite", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			planeTarget, 250, 250, 1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Y);
	}
}

void NpcShipRO::buildNode(int entityIndex)
{
	ShipRO::buildNode(entityIndex);
	mp_frameNode = getSceneManager()->getRootSceneNode()->createChildSceneNode();

	std::stringstream oss;
	oss << "TargetFrame" << entityIndex;
	mp_frameSprite = getSceneManager()->createEntity(oss.str(), "targetFrameSprite");
	mp_frameSprite->setMaterialName("Orewar/TargetFrame");
	mp_frameSprite->setCastShadows(false);
	mp_frameNode->attachObject(mp_frameSprite);
}

void NpcShipRO::destroyNode()
{
	ShipRO::destroyNode();
	mp_frameNode->detachAllObjects();
	mp_frameNode->removeAllChildren();
	getSceneManager()->destroyMovableObject(mp_frameSprite);
	getSceneManager()->destroySceneNode(mp_frameNode);
}

// ========================================================================
// ProjectileRO Implementation
// ========================================================================
bool ProjectileRO::m_resourcesLoaded = false;

ProjectileRO::ProjectileRO(PhysicsObject * object, SceneManager * mgr)
	: RenderObject(object, mgr), mp_projNode(NULL), mp_projSprite(NULL), mp_pointLight(NULL)
{
}

void ProjectileRO::updateNode(Real elapsedTime, Quaternion camOrientation)
{
	mp_projNode->setPosition(getObject()->getPosition());
	mp_projNode->setOrientation(camOrientation);
}


void ProjectileRO::loadSceneResources()
{
	if(!m_resourcesLoaded) {
		Plane planePlasma = Plane(Ogre::Vector3::UNIT_Z, 0);
		MeshManager::getSingleton().createPlane("projSprite", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			planePlasma, 100, 100, 1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Y);
		m_resourcesLoaded = true;
	}
}

void ProjectileRO::buildNode(int entityIndex)
{
	mp_projNode = getSceneManager()->getRootSceneNode()->createChildSceneNode();

	std::stringstream oss;
	oss << "Projectile" << entityIndex;
	mp_projSprite = getSceneManager()->createEntity(oss.str(), "projSprite");
	mp_projSprite->setMaterialName("Orewar/PlasmaSprite");
	mp_projSprite->setCastShadows(false);
	mp_projNode->attachObject(mp_projSprite);

	// Dynamic point lights on projectiles
	oss << "Light";
	mp_pointLight = getSceneManager()->createLight(oss.str());
	mp_pointLight->setType(Ogre::Light::LT_POINT);
	mp_pointLight->setPosition(Ogre::Vector3(0, 60, 0));
	mp_pointLight->setDiffuseColour(0.0, 1, 0.0);
	mp_pointLight->setSpecularColour(0.2, 0.7, 0.2);
	mp_pointLight->setAttenuation(3250, 1.0, 0.0014, 0.000007);
	mp_pointLight->setCastShadows(false);
	mp_projNode->attachObject(mp_pointLight);
}

void ProjectileRO::destroyNode()
{
	mp_projNode->detachAllObjects();
	mp_projNode->removeAllChildren();
	getSceneManager()->destroyMovableObject(mp_projSprite);
	getSceneManager()->destroyLight(mp_pointLight);
	getSceneManager()->destroySceneNode(mp_projNode);
}

// ========================================================================
// RenderModel Implementation
// ========================================================================
RenderModel::RenderModel(GameArena& model, SceneManager * mgr) : m_model(model), m_renderList(),
	mp_mgr(mgr), m_entityIndex(0)
{
	m_model.addGameArenaListener(this);
}

void RenderModel::generateRenderObject(PhysicsObject * object)
{
	// TODO: Resource construction / release should all be in virtual methods
	// of RenderObject subclasses
	RenderObject * p_renderObj = NULL;
	if(object->getType() == ObjectType::SHIP) {
		p_renderObj = new ShipRO(object, mp_mgr);
	} else if (object->getType() == ObjectType::NPC_SHIP) {
		p_renderObj = new NpcShipRO(object, mp_mgr);
	} else if (object->getType() == ObjectType::PROJECTILE) {
		p_renderObj = new ProjectileRO(object, mp_mgr);
	}

	p_renderObj->loadSceneResources();
	p_renderObj->buildNode(m_entityIndex);
	m_renderList.push_back(p_renderObj);
	m_entityIndex++;
}

void RenderModel::destroyRenderObject(PhysicsObject * object)
{
	for(std::vector<RenderObject *>::iterator renderIter =  m_renderList.begin(); 
		renderIter != m_renderList.end();
		renderIter++) {

		if((*(*renderIter)).getObject() == object) {
			(*(*renderIter)).destroyNode();
			delete (*renderIter);
			m_renderList.erase(std::remove(m_renderList.begin(), m_renderList.end(), (*renderIter)), m_renderList.end());
			return;
		}
	}
}

void RenderModel::updateRenderList(Real elapsedTime, Quaternion camOrientation)
{
	for(std::vector<RenderObject *>::iterator renderIter =  m_renderList.begin(); 
		renderIter != m_renderList.end();
		renderIter++) {
		(*(*renderIter)).updateNode(elapsedTime, camOrientation);
	}
}

void RenderModel::newPhysicsObject(PhysicsObject * object)
{
	generateRenderObject(object);
}

void RenderModel::destroyedPhysicsObject(PhysicsObject * object)
{
	destroyRenderObject(object);
}

int RenderModel::getNumObjects() const
{
	return m_renderList.size();
}