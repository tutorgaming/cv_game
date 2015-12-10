#include "Bullet.hpp"


Bullet::Bullet(void)
{
	m_fieldX = 1000;
	m_fieldY = 1000;
	m_fieldZ = 1000;
}

Bullet::~Bullet(void)
{
	// Destroy all the attached objects
	//DestroyAllAttachedMovableObjects(m_pBulletNode);

	m_pSceneMgr->getRootSceneNode()->removeChild(m_pBulletNode);
}

void Bullet::create(int xPos, int yPos)
{
	// Create the entity
	m_pBulletEntity = m_pSceneMgr->createEntity("Cube02.mesh");
	// Create the scene node
	m_pBulletNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0, 0, 0.0f));
	m_pBulletNode->attachObject(m_pBulletEntity);

	//m_BulletInfo.mpNode = m_pBulletNode;


	m_pBulletNode->setScale(3, 3, 3);
	m_pBulletNode->pitch(Ogre::Radian(Ogre::Degree(90)));
	m_pBulletNode->yaw(Ogre::Radian(Ogre::Degree(90)));


	reset(xPos, yPos);

	m_BulletOrientation = 0.0;
	m_BulletAcceleration = 0.001;
	m_BulletMaxSpeed = 0.9;
	m_BulletSpeed = Ogre::Vector3(0.0, 0.0, -0.9);
	m_BulletRotateSpeed = 0.2;
	m_BulletRotation = 0.0;

	m_active_flag = true;
}

void Bullet::die()
{
	//TODO
	//A explosion would be nice ;)
	m_pBulletNode->setVisible(false);

	m_active_flag = false;
}

void Bullet::reset(int xPos, int yPos)
{
	m_pBulletNode->setVisible(true);
	m_active_flag = true;

	m_pBulletNode->setPosition(Ogre::Vector3(xPos, yPos, 0));
}


void Bullet::move(double timeSinceLastFrame)
{
	//Move
	Ogre::Vector3 BulletMovement = m_BulletSpeed * timeSinceLastFrame;
	m_pBulletNode->translate(BulletMovement);

	//Are we out of the Game Field?
	Ogre::Vector3	position = m_pBulletNode->getPosition();

	if (position.x >= m_fieldX)	position.x = m_fieldX;
	if (position.x <= -m_fieldX)	position.x = -m_fieldX;
	if (position.y >= m_fieldY)	position.y = m_fieldY;
	if (position.y <= -m_fieldY)	position.y = -m_fieldY;
	if (position.z <= -m_fieldZ)	{
		die();
	}
	if (position.z >= m_fieldZ)	{
		position.z = m_fieldZ;
	}

	if (position != m_pBulletNode->getPosition())
		m_pBulletNode->setPosition(position);

}