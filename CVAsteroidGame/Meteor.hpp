#ifndef METEOR
#define METEOR

#include <OgreVector3.h>
#include <math.h>

#include "GameState.hpp"

#define C_ZBOUND 100
#define ZSTART -200

class Meteor
{
public:
	Meteor(Ogre::SceneManager* m_pSceneMgr);
	void update(double timeSinceLastFrame);
	bool isOutOfSpace();
private:
	Ogre::Real c_lb;
	Ogre::Vector3 m_position;
	Ogre::Vector3 m_direction;
	Ogre::Vector3 m_velocity;
	Ogre::Entity* m_entity;
	Ogre::SceneNode* m_node;
};

#endif