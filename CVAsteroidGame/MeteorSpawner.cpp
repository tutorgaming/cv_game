#include "MeteorSpawner.hpp"

MeteorSpawner::MeteorSpawner(Ogre::SceneManager* m_pSceneMgr)
{
	m_meteorList = std::vector<Meteor*>();

	this->m_pSceneMgr = m_pSceneMgr;

}

void MeteorSpawner::generateMeteor(int amount)
{
	for (int i = 0; i < amount; i++)
	{
		createMeteor();
	}
}

void MeteorSpawner::createMeteor()
{
	Meteor* temp = new Meteor(m_pSceneMgr);
	m_meteorList.push_back(temp);
}

void MeteorSpawner::updateMeteor(double timeSinceLastFrame)
{
	for each (Meteor* meteor in m_meteorList)
	{
		meteor->update(timeSinceLastFrame);
	}

	for each (Meteor* meteor in m_meteorList)
	{
		if (meteor->isOutOfSpace())
		{
			m_meteorList.erase(std::remove(m_meteorList.begin(), m_meteorList.end(), meteor), m_meteorList.end());
			delete meteor;
		}
	}
}