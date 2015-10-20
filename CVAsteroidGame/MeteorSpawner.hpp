
#ifndef METEOR_SPAWNER
#define METEOR_SPAWNER

#include <vector>
#include "Meteor.hpp"

class MeteorSpawner
{
public:
	MeteorSpawner(Ogre::SceneManager* m_pSceneMgr);

	void generateMeteor(int amount);
	void createMeteor();
	void updateMeteor(double timeSinceLastFrame);
private:
	std::vector<Meteor*> m_meteorList;
	Ogre::SceneManager* m_pSceneMgr;
};

#endif