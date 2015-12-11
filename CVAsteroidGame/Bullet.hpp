#include <math.h>
#include "OgreManualObject.h"
#include "OgreMaterialManager.h"
#include "AdvancedOgreFramework.hpp"
#include "OgreParticleSystem.h"
#include <vector>

class Bullet
{


private:
	Ogre::Entity*			m_pBulletEntity;
	Ogre::SceneNode*		m_pBulletNode;

	Ogre::Real				m_fieldX, m_fieldY, m_fieldZ;

	Ogre::Vector3			m_BulletSpeed;
	Ogre::Degree			m_BulletOrientation;
	Ogre::Real				m_BulletAcceleration;
	Ogre::Real				m_BulletMaxSpeed;
	Ogre::Degree			m_BulletRotateSpeed;
	Ogre::Degree			m_BulletRotation;

	double					m_BulletRadius;

	Ogre::SceneManager*		m_pSceneMgr;

	bool					m_active_flag;

public:
	Bullet(void);
	~Bullet(void);

	void move(double timeSinceLastFrame);
	void setSceneManager(Ogre::SceneManager*		pSceneMgr){ m_pSceneMgr = pSceneMgr; }
	void create(int xPos, int yPos, Ogre::Vector3 direction);
	void die();
	void reset(int xPos, int yPos, Ogre::Vector3 direction);
	bool isActive(){ return m_active_flag; }

	void getPoint(const Ogre::Vector3* v);

	void setGameField(Ogre::Real x, Ogre::Real y){ m_fieldX = x; m_fieldY = y; }
	Ogre::Vector3 getPosition() { return m_pBulletNode->getPosition(); };
};