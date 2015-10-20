#include "Meteor.hpp"

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

void Meteor::update(double timeSinceLastFrame)
{
	m_node->translate(timeSinceLastFrame * m_velocity * m_direction);
}

bool Meteor::isOutOfSpace(){
	return m_position.y >= C_ZBOUND;
}