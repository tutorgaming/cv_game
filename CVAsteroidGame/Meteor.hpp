#include <math.h>
#include "OgreManualObject.h"
#include "OgreMaterialManager.h"
#include "AdvancedOgreFramework.hpp"
#include "OgreParticleSystem.h"


class Meteor
{

public:
	Meteor(void);
	~Meteor(void);

	void move(double timeSinceLastFrame);
	void setSceneManager(Ogre::SceneManager*		pSceneMgr){ m_pSceneMgr = pSceneMgr; }
	void create();
	void die();
	void reset();
	bool isActive(){ return m_active_flag; }
	bool isIn(Ogre::Vector3 point);

	void setGameField(Ogre::Real x, Ogre::Real y){ m_fieldX = x; m_fieldY = y; }
private:
	Ogre::Entity*			m_pMeteorEntity;
	Ogre::SceneNode*		m_pMeteorNode;

	Ogre::Real			m_fieldX, m_fieldY, m_fieldZ;

	Ogre::Vector3			m_MeteorSpeed;
	Ogre::Degree			m_MeteorOrientation;
	Ogre::Real			m_MeteorAcceleration;
	Ogre::Real			m_MeteorMaxSpeed;
	Ogre::Degree			m_MeteorRotateSpeed;
	Ogre::Degree			m_MeteorRotation;

	double				m_MeteorRadius;

	Ogre::SceneManager*	m_pSceneMgr;

	bool				m_active_flag;
};
