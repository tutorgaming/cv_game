//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AppState.hpp"

#include "DotSceneLoader.hpp"

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>

#include "Meteor.hpp"
#include "Bullet.hpp"

#include <vector>

//|||||||||||||||||||||||||||||||||||||||||||||||

enum QueryFlags
{
	OGRE_HEAD_MASK	= 1<<0,
    CUBE_MASK		= 1<<1
};

//|||||||||||||||||||||||||||||||||||||||||||||||

class GameState : public AppState
{
public:
	GameState();

	DECLARE_APPSTATE_CLASS(GameState)

	void enter();
	void createScene();
	void exit();
	bool pause();
	void resume();

	void moveCamera();
	void getInput();
    void buildGUI();

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

	void onLeftPressed(const OIS::MouseEvent &evt);
    void itemSelected(OgreBites::SelectMenu* menu);

	void moveByHeadPose();
	void moveByHeadPosition();
	void moveCursorByHeadPose();

	//Check the Bullet 
	void checkShoot();

	void update(double timeSinceLastFrame);

	//Meteor Spawning and Pool Manager
	void spawnMeteor();
	void updateMeteor(double timeSinceLastFrame);
	void checkGenerateMeteor(double timeSinceLastFrame);
	void randomSpawnDelay();

	//Bullet Spawning and Pool Manager
	void spawnBullet(int xPos, int yPos);
	void updateBullet(double timeSinceLastFrame);
	void checkGenerateBullet(double timeSinceLastFrame);

	//Game System
	bool isIntersect(Meteor* m, Bullet* b);
	void upScore();

	//Calibrate crosshair
	void calibrateCrossHair();

private:
	Ogre::SceneNode*			m_pOgreHeadNode;
	Ogre::Entity*				m_pOgreHeadEntity;
	Ogre::MaterialPtr			m_pOgreHeadMat;
	Ogre::MaterialPtr			m_pOgreHeadMatHigh;

    OgreBites::ParamsPanel*		m_pDetailsPanel;
	bool						m_bQuit;

	Ogre::Vector3				m_TranslateVector;
	Ogre::Real					m_MoveSpeed;
	Ogre::Degree				m_RotateSpeed;
	float						m_MoveScale;
	Ogre::Degree				m_RotScale;

	Ogre::RaySceneQuery*		m_pRSQ;
	Ogre::SceneNode*			m_pCurrentObject;
	Ogre::Entity*				m_pCurrentEntity;
	bool						m_bLMouseDown, m_bRMouseDown;
	bool						m_bSettingsMode;

	float						m_spawnElapsedTime;
	float						m_spawnRndDelay;
	float						m_spawnMinDelay;
	float						m_spawnMaxDelay;

	float						m_bulletElapsedTime;
	float						m_bulletDelay;

	std::vector<Bullet*>		m_bulletList;
	std::vector<Meteor*>		m_meteorList;

	// Camera
	Ogre::Vector3				m_lookPosition;

	// Head Pose
	Ogre::Matrix3				m_LastHeadPose;
	Ogre::Vector3				m_camTargetPos;
	Ogre::Vector3				m_currentLookPos;
	float						m_headMoveScale;

	//Shoot Pose
	Ogre::Vector2				m_shootPos;
	
	//Game System
	int							m_score;
	int							m_hitPoint;
	int							m_manaPoint;

	//Cursor Position
	Ogre::Vector3				mousePosition;

	// Calibrate cross-hair
	int m_crossOffsetX;
	int m_crossOffsetY;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||