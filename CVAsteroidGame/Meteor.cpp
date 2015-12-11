#include "Meteor.hpp"


Meteor::Meteor(void)
{
	m_fieldX = 400;
	m_fieldY = 200;
	m_fieldZ = 1000;
}

Meteor::~Meteor(void)
{
	// Destroy all the attached objects
	//DestroyAllAttachedMovableObjects(m_pMeteorNode);

	m_pSceneMgr->getRootSceneNode()->removeChild(m_pMeteorNode);
}

void Meteor::create()
{
	// Create the entity
	m_pMeteorEntity = m_pSceneMgr->createEntity("Cube01.mesh");
	// Create the scene node
	m_pMeteorNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0, 0, -100.0f));
	m_pMeteorNode->attachObject(m_pMeteorEntity);

	//m_MeteorInfo.mpNode = m_pMeteorNode;


	m_pMeteorNode->setScale(3, 3, 3);
	m_pMeteorNode->pitch(Ogre::Radian(Ogre::Degree(90)));
	m_pMeteorNode->yaw(Ogre::Radian(Ogre::Degree(90)));


	reset();

	m_MeteorOrientation = 0.0;
	m_MeteorAcceleration = 0.001;
	m_MeteorMaxSpeed = 0.9;
	m_MeteorSpeed = Ogre::Vector3(0.0, 0.0, 0.9);
	m_MeteorRotateSpeed = 0.2;
	m_MeteorRotation = 0.0;

	m_active_flag = true;
}

void Meteor::die()
{
	//TODO
	//A explosion would be nice ;)
	m_pMeteorNode->setVisible(false);

	m_active_flag = false;
}

void Meteor::reset()
{
	m_pMeteorNode->setVisible(true);
	m_active_flag = true;
	int n = 3, gridSize = 30;
	int offset = (n - 1) / 2;
	int xRand = (int)(Ogre::Math::UnitRandom() * n) - offset;
	
	int yRand = (int)(Ogre::Math::UnitRandom() * n) - offset;

	m_pMeteorNode->setPosition(Ogre::Vector3(xRand * gridSize, yRand * gridSize, -m_fieldZ));
}


void Meteor::move(double timeSinceLastFrame)
{
	//Move
	Ogre::Vector3 MeteorMovement = m_MeteorSpeed * timeSinceLastFrame;
	m_pMeteorNode->translate(MeteorMovement);

	//Are we out of the Game Field?
	Ogre::Vector3	position = m_pMeteorNode->getPosition();

	if (position.x > m_fieldX)	position.x = m_fieldX;
	if (position.x < -m_fieldX)	position.x = -m_fieldX;
	if (position.y > m_fieldY)	position.y = m_fieldY;
	if (position.y < -m_fieldY)	position.y = -m_fieldY;
	if (position.z > m_fieldZ)	{
		die();
	}
	if (position.z < -m_fieldZ)	{
		position.z = -m_fieldZ;
		
	}
	if (position != m_pMeteorNode->getPosition())
		m_pMeteorNode->setPosition(position);

}

/*
Meteor::Meteor(Ogre::SceneManager* m_pSceneMgr){
	m_entity = m_pSceneMgr->createEntity("Cube01.mesh");

	m_entity->setQueryFlags(CUBE_MASK);

	float xRand = Ogre::Math::RangeRandom(-10.0, 10.0);
	float yRand = Ogre::Math::RangeRandom(-10.0, 10.0);

	m_position = Ogre::Vector3(xRand, yRand, ZSTART);
	m_velocity = Ogre::Vector3(0, 0, 1);
	m_direction = Ogre::Vector3(0, 0, 1);

	m_node = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(
		Ogre::Vector3(xRand, yRand, -200.0));
	m_node->attachObject(m_entity);


}
*/

bool Meteor::isIn(Ogre::Vector3 point)
{
	return m_pMeteorEntity->getWorldBoundingBox(true).contains(point);
	/*Ogre::Vector3 size = m_pMeteorEntity->getBoundingBox().getSize();

	Ogre::Vector3 position = m_pMeteorNode->getPosition();

	return position.x - size.x / 2 <= point.x && point.x <= position.x + size.x / 2 &&
		position.y - size.y / 2 <= point.y && point.y <= position.y + size.y / 2 &&
		position.z - size.z / 2 <= point.z && point.z <= position.z + size.z / 2 ;*/
}
