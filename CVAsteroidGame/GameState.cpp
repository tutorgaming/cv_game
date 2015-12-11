//|||||||||||||||||||||||||||||||||||||||||||||||

#include "GameState.hpp"
#include "math.h"
#include "time.h"
#include "CVProcess.hpp"

//|||||||||||||||||||||||||||||||||||||||||||||||

Ogre::Entity* cube;
Ogre::SceneNode* cubeNode;
Ogre::Vector3 mPosition;
Ogre::Vector3 mDirection;
bool m_move;

//|||||||||||||||||||||||||||||||||||||||||||||||

GameState::GameState()
{
    m_MoveSpeed			= 0.1f;
    m_RotateSpeed		= 0.3f;

    m_bLMouseDown       = false;
    m_bRMouseDown       = false;
    m_bQuit             = false;
    m_bSettingsMode     = false;


    m_pDetailsPanel		= 0;

	m_lookPosition = Ogre::Vector3(0, 0, -20);
	m_currentLookPos = m_lookPosition;

	m_crossOffsetX = 0;
	m_crossOffsetY = 0;

}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::enter()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering GameState...");

	m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(Ogre::ST_GENERIC, "GameSceneMgr");
    m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

    m_pSceneMgr->addRenderQueueListener(OgreFramework::getSingletonPtr()->m_pOverlaySystem);

	m_pRSQ = m_pSceneMgr->createRayQuery(Ogre::Ray());
    m_pRSQ->setQueryMask(OGRE_HEAD_MASK);

    m_pCamera = m_pSceneMgr->createCamera("GameCamera");

	m_pCamera->setPosition(Ogre::Vector3(0, 0, 0));
    m_pCamera->lookAt(m_lookPosition);

    m_pCamera->setNearClipDistance(5);

	m_pCamera->setAspectRatio(Ogre::Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth()) /
		Ogre::Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight()));

    OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
    m_pCurrentObject = 0;

	srand(time(NULL));

    buildGUI();

    createScene();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::pause()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Pausing GameState...");

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::resume()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Resuming GameState...");

    buildGUI();

    OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
    m_bQuit = false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::exit()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving GameState...");

    m_pSceneMgr->destroyCamera(m_pCamera);
    m_pSceneMgr->destroyQuery(m_pRSQ);
    if(m_pSceneMgr)
        OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::createScene()
{
    m_pSceneMgr->createLight("Light")->setPosition(75,75,75);

    DotSceneLoader* pDotSceneLoader = new DotSceneLoader();
    pDotSceneLoader->parseDotScene("CubeScene.xml", "General", m_pSceneMgr, m_pSceneMgr->getRootSceneNode());
    delete pDotSceneLoader;

	m_spawnMaxDelay = 1200;
	m_spawnMinDelay = 500;
	randomSpawnDelay();
	m_spawnElapsedTime = 0;
	m_bulletDelay = 300;
	m_bulletElapsedTime = 0;

	m_headMoveScale = 150;

	//Create Mouse Cursor
	
	m_score = 0;
	m_hitPoint = m_maxHitPoint;
	m_manaPoint = m_maxManaPoint;
	m_maxHitPoint = 1000;
	m_damagePerHit = 200;
	m_manaPerShot = 200;
	m_manaPerTime = 50;
	m_maxManaPoint = 1000;
	m_isAlive = true;

	/*
	m_pSceneMgr->getEntity("Cube01")->setQueryFlags(CUBE_MASK);
    m_pSceneMgr->getEntity("Cube02")->setQueryFlags(CUBE_MASK);
    m_pSceneMgr->getEntity("Cube03")->setQueryFlags(CUBE_MASK);
	*/
	// OGRE HEAD
	/*
    m_pOgreHeadEntity = m_pSceneMgr->createEntity("OgreHeadEntity", "ogrehead.mesh");
    m_pOgreHeadEntity->setQueryFlags(OGRE_HEAD_MASK);
    m_pOgreHeadNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("OgreHeadNode");
    m_pOgreHeadNode->attachObject(m_pOgreHeadEntity);
    m_pOgreHeadNode->setPosition(Vector3(0, 0, -25));

    m_pOgreHeadMat = m_pOgreHeadEntity->getSubEntity(1)->getMaterial();
    m_pOgreHeadMatHigh = m_pOgreHeadMat->clone("OgreHeadMatHigh");
    m_pOgreHeadMatHigh->getTechnique(0)->getPass(0)->setAmbient(1, 0, 0);
    m_pOgreHeadMatHigh->getTechnique(0)->getPass(0)->setDiffuse(1, 0, 0, 0);
	*/
	
	m_pSceneMgr->setSkyBox(true, "SkyBox/Space", 1000);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    if(m_bSettingsMode == true)
    {
        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_S))
        {
            OgreBites::SelectMenu* pMenu = (OgreBites::SelectMenu*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("DisplayModeSelMenu");
            if(pMenu->getSelectionIndex() + 1 < (int)pMenu->getNumItems())
                pMenu->selectItem(pMenu->getSelectionIndex() + 1);
        }

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_W))
        {
            OgreBites::SelectMenu* pMenu = (OgreBites::SelectMenu*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("DisplayModeSelMenu");
            if(pMenu->getSelectionIndex() - 1 >= 0)
                pMenu->selectItem(pMenu->getSelectionIndex() - 1);
        }
    }

    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
    {
        pushAppState(findByName("PauseState"));
        return true;
    }

	// Press spacebar to calibrate crosshair
	if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_SPACE))
	{
		calibrateCrossHair();
		return true;
	}

    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_I))
    {
        if(m_pDetailsPanel->getTrayLocation() == OgreBites::TL_NONE)
        {
            OgreFramework::getSingletonPtr()->m_pTrayMgr->moveWidgetToTray(m_pDetailsPanel, OgreBites::TL_TOPLEFT, 0);
            m_pDetailsPanel->show();
        }
        else
        {
            OgreFramework::getSingletonPtr()->m_pTrayMgr->removeWidgetFromTray(m_pDetailsPanel);
            m_pDetailsPanel->hide();
        }
    }

    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_TAB))
    {
        m_bSettingsMode = !m_bSettingsMode;
        return true;
    }

    if(m_bSettingsMode && OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_RETURN) ||
        OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_NUMPADENTER))
    {
    }

    if(!m_bSettingsMode || (m_bSettingsMode && !OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_O)))
        OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mouseMoved(const OIS::MouseEvent &evt)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseMove(evt)) return true;

    if(m_bRMouseDown)
    {
		m_pCamera->yaw(Ogre::Degree(evt.state.X.rel * -0.1f));
		m_pCamera->pitch(Ogre::Degree(evt.state.Y.rel * -0.1f));
    }


	if(m_bLMouseDown)
	{
		m_shootPos.x = mousePosition.x;
		m_shootPos.y = mousePosition.y;
	}
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseDown(evt, id)) return true;

    if(id == OIS::MB_Left)
    {
        //onLeftPressed(evt);
        m_bLMouseDown = true;
		m_bulletElapsedTime = 400;
		m_shootPos.x = mousePosition.x;
		m_shootPos.y = mousePosition.y;
    }
    else if(id == OIS::MB_Right)
    {
        m_bRMouseDown = true;
    }

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseUp(evt, id)) return true;

    if(id == OIS::MB_Left)
    {
        m_bLMouseDown = false;
    }
    else if(id == OIS::MB_Right)
    {
        m_bRMouseDown = false;
    }

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::onLeftPressed(const OIS::MouseEvent &evt)
{
    if(m_pCurrentObject)
    {
        m_pCurrentObject->showBoundingBox(false);
        m_pCurrentEntity->getSubEntity(1)->setMaterial(m_pOgreHeadMat);
    }

    Ogre::Ray mouseRay = m_pCamera->getCameraToViewportRay(OgreFramework::getSingletonPtr()->m_pMouse->getMouseState().X.abs / float(evt.state.width),
        OgreFramework::getSingletonPtr()->m_pMouse->getMouseState().Y.abs / float(evt.state.height));
    m_pRSQ->setRay(mouseRay);
    m_pRSQ->setSortByDistance(true);

    Ogre::RaySceneQueryResult &result = m_pRSQ->execute();
    Ogre::RaySceneQueryResult::iterator itr;

    for(itr = result.begin(); itr != result.end(); itr++)
    {
        if(itr->movable)
        {
            OgreFramework::getSingletonPtr()->m_pLog->logMessage("MovableName: " + itr->movable->getName());
            m_pCurrentObject = m_pSceneMgr->getEntity(itr->movable->getName())->getParentSceneNode();
            OgreFramework::getSingletonPtr()->m_pLog->logMessage("ObjName " + m_pCurrentObject->getName());
            m_pCurrentObject->showBoundingBox(true);
            m_pCurrentEntity = m_pSceneMgr->getEntity(itr->movable->getName());
            //m_pCurrentEntity->getSubEntity(1)->setMaterial(m_pOgreHeadMatHigh);
            break;
        }
    }
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::moveCamera()
{
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_LSHIFT))
        m_pCamera->moveRelative(m_TranslateVector);
    m_pCamera->moveRelative(m_TranslateVector / 10);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::getInput()
{
    if(m_bSettingsMode == false)
    {
        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_A))
            m_TranslateVector.x = -m_MoveScale;

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_D))
            m_TranslateVector.x = m_MoveScale;

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_W))
            m_TranslateVector.y = m_MoveScale;

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_S))
            m_TranslateVector.y = -m_MoveScale;

		if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_G))
			spawnMeteor();
    }
}
//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::calibrateCrossHair()
{
	float* lookPosition = (CVProcess::getInstance().mHeadPose)->getLookPosition();
	int x = (int)(lookPosition[0] * OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth() + OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth() * 0.5f);
	int y = (int)(lookPosition[1] * OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight() * 2 + OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight() * 0.5f);
	m_crossOffsetX = OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth() * 0.5f - x;
	m_crossOffsetY = OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight() * 0.5f - y;
}

void GameState::moveByHeadPosition()
{
	float* headPos = CVProcess::getInstance().mHeadPose->getHeadPosition();
	m_camTargetPos = Ogre::Vector3(headPos[0] * m_headMoveScale, headPos[1] * m_headMoveScale, m_pCamera->getPosition().z);
	
	Ogre::Vector3 smoothCamPos = m_pCamera->getPosition() + 0.2 * (m_camTargetPos - m_pCamera->getPosition());

	m_pCamera->setPosition(smoothCamPos);
}

void GameState::moveByHeadPose()
{
	
	float* rotMat = (CVProcess::getInstance().mHeadPose)->getHeadRotationMatrix();
	Ogre::Matrix3 currentRotationMatrix(
		rotMat[0], rotMat[1], rotMat[2],
		rotMat[3], rotMat[4], rotMat[5],
		rotMat[6], rotMat[7], rotMat[8]); //FIXED STABLE HERE FOR FIXED VIEW

	Ogre::Vector3 camPos = m_pCamera->getPosition();

	Ogre::Vector3 newLookPos;
	newLookPos.x = rotMat[0] * m_lookPosition.x +
		rotMat[1] * m_lookPosition.y +
		rotMat[2] * m_lookPosition.z;
	newLookPos.y = rotMat[3] * m_lookPosition.x +
		rotMat[4] * m_lookPosition.y +
		rotMat[5] * m_lookPosition.z;
	newLookPos.z = rotMat[6] * m_lookPosition.x +
		rotMat[7] * m_lookPosition.y +
		rotMat[8] * m_lookPosition.z;

	newLookPos.z *= -1;
	//newLookPos.y *= -1;
	newLookPos.x *= -1;

	//Ogre::Radian euler_x, euler_y, euler_z;
	//currentRotationMatrix.ToEulerAnglesXYZ(euler_x, euler_y, euler_z);

	//m_pCamera->yaw(euler_x);
	//m_pCamera->pitch(euler_y);

	/*Ogre::Quaternion cur_Q = Ogre::Quaternion(currentRotationMatrix);
	Ogre::Quaternion last_Q = Ogre::Quaternion(m_LastHeadPose);

	Ogre::Radian rel_pitch = cur_Q.getPitch() - last_Q.getPitch();
	Ogre::Radian rel_yaw = cur_Q.getYaw() - last_Q.getYaw();*/
	//m_pCamera->lookAt()
	//m_pCamera->setOrientation(Ogre::Quaternion(currentRotationMatrix));


	Ogre::Vector3 smoothLookPos = m_currentLookPos + 0.02 * (newLookPos - m_currentLookPos);

	m_currentLookPos = smoothLookPos;
	
	m_pCamera->lookAt(smoothLookPos + camPos);

	m_LastHeadPose = currentRotationMatrix;
	
}

//CURSOR CONTROL
void GameState::moveCursorByHeadPose()
{
	
	float* rotMat = (CVProcess::getInstance().mHeadPose)->getHeadRotationMatrix();
	Ogre::Matrix3 currentRotationMatrix(
		rotMat[0], rotMat[1], rotMat[2],
		rotMat[3], rotMat[4], rotMat[5],
		rotMat[6], rotMat[7], rotMat[8]); //FIXED STABLE HERE FOR FIXED VIEW

	Ogre::Vector3 camPos = m_pCamera->getPosition();

	Ogre::Vector3 newLookPos;
	newLookPos.x = rotMat[0] * m_lookPosition.x +
		rotMat[1] * m_lookPosition.y +
		rotMat[2] * m_lookPosition.z;
	newLookPos.y = rotMat[3] * m_lookPosition.x +
		rotMat[4] * m_lookPosition.y +
		rotMat[5] * m_lookPosition.z;
	newLookPos.z = rotMat[6] * m_lookPosition.x +
		rotMat[7] * m_lookPosition.y +
		rotMat[8] * m_lookPosition.z;

	newLookPos.z *= -1;
	//newLookPos.y *= -1;
	newLookPos.x *= -1;

	Ogre::Vector3 smoothLookPos = m_currentLookPos + 0.02 * (newLookPos - m_currentLookPos);

	m_currentLookPos = smoothLookPos;
	
	//Ogre::Vector3 *mousePosition = new Ogre::Vector3(m_currentLookPos.x,-m_currentLookPos.y,0.0f);
	//Receiving 
	float* lookPosition = (CVProcess::getInstance().mHeadPose)->getLookPosition();
	mousePosition.x = lookPosition[0] * OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth()  + OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth() * 0.5f + m_crossOffsetX;
	mousePosition.y = lookPosition[1] * OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight() * 2 + OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight() * 0.5f + +m_crossOffsetY;
	mousePosition.z = 0.0f;

	//Boundary
	if( mousePosition.x >= OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth()){
		mousePosition.x = OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth();
	}
	if( mousePosition.x < 0){
		mousePosition.x = 0;
	}
	if( mousePosition.y >= OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight()){
		mousePosition.y = OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight();
	}
	if( mousePosition.y < 0){
		mousePosition.y = 0;
	}

	//m_pCamera->lookAt(smoothLookPos + camPos);
	OgreFramework::getSingletonPtr()->
		m_pTrayMgr->getCursorContainer()->
		setPosition(
		mousePosition.x
		, mousePosition.y
		);
	

	m_LastHeadPose = currentRotationMatrix;
	
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::spawnMeteor()
{
	//for each(Meteor* m in m_meteorList)
	for (auto m:m_meteorList)
	{
		if (!m->isActive())
		{
			m->reset();//Will Be Optimized  to objPool Soon
			return;
		}
	}
	Meteor *m = new Meteor();
	m_meteorList.push_back(m);
	m->setSceneManager(m_pSceneMgr);
	m->create();
}

void GameState::update(double timeSinceLastFrame)
{
    m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    OgreFramework::getSingletonPtr()->m_pTrayMgr->frameRenderingQueued(m_FrameEvent);

    if(m_bQuit == true)
    {
        popAppState();
        return;
    }

    if(!OgreFramework::getSingletonPtr()->m_pTrayMgr->isDialogVisible())
    {
        if(m_pDetailsPanel->isVisible())
        {
            m_pDetailsPanel->setParamValue(0, Ogre::StringConverter::toString(m_pCamera->getDerivedPosition().x));
            m_pDetailsPanel->setParamValue(1, Ogre::StringConverter::toString(m_pCamera->getDerivedPosition().y));
            m_pDetailsPanel->setParamValue(2, Ogre::StringConverter::toString(m_pCamera->getDerivedPosition().z));
            m_pDetailsPanel->setParamValue(3, Ogre::StringConverter::toString(m_pCamera->getDerivedOrientation().w));
            m_pDetailsPanel->setParamValue(4, Ogre::StringConverter::toString(m_pCamera->getDerivedOrientation().x));
            m_pDetailsPanel->setParamValue(5, Ogre::StringConverter::toString(m_pCamera->getDerivedOrientation().y));
            m_pDetailsPanel->setParamValue(6, Ogre::StringConverter::toString(m_pCamera->getDerivedOrientation().z));

			m_pDetailsPanel->setParamValue(7, Ogre::StringConverter::toString(mousePosition.x));
			m_pDetailsPanel->setParamValue(8, Ogre::StringConverter::toString(mousePosition.y));
			m_pDetailsPanel->setParamValue(9, Ogre::StringConverter::toString(m_score));
			m_pDetailsPanel->setParamValue(10, Ogre::StringConverter::toString(m_hitPoint) + "/" + Ogre::StringConverter::toString(m_maxHitPoint));
			m_pDetailsPanel->setParamValue(11, Ogre::StringConverter::toString(m_manaPoint) + "/" + Ogre::StringConverter::toString(m_maxManaPoint));
			if(m_bSettingsMode)
                m_pDetailsPanel->setParamValue(11, "Buffered Input");
            else
                m_pDetailsPanel->setParamValue(11, "Un-Buffered Input");
        }
    }

    m_MoveScale = m_MoveSpeed   * timeSinceLastFrame;
    m_RotScale  = m_RotateSpeed * timeSinceLastFrame;

    m_TranslateVector = Ogre::Vector3::ZERO;
	

    getInput();
    
	moveCamera();
	

	moveByHeadPosition();

	// Move camera by head pose
	//moveByHeadPose();
	moveCursorByHeadPose();

	// Generate meteor
	checkGenerateMeteor(timeSinceLastFrame);
	checkGenerateBullet(timeSinceLastFrame);

	updateMeteor(timeSinceLastFrame);
	updateBullet(timeSinceLastFrame);
}

void GameState::checkGenerateMeteor(double timeSinceLastFrame)
{
	m_spawnElapsedTime += (float)timeSinceLastFrame;
	if (m_spawnElapsedTime >= m_spawnRndDelay)
	{
		m_spawnElapsedTime = 0;
		randomSpawnDelay();
		for (int i = 0; i < 3; i++)
			spawnMeteor();
	}
}

void GameState::randomSpawnDelay()
{
	m_spawnRndDelay = (float)Ogre::Math::RangeRandom(m_spawnMinDelay, m_spawnMaxDelay);
	//m_spawnRndDelay = 3000;
}

void GameState::updateMeteor(double timeSinceLastFrame)
{
	
	for(auto m : m_meteorList)
	//for each(Meteor* m in m_meteorList)
	{
		if (m->isActive()){
			m->move(timeSinceLastFrame);
			if (isCrashPlayer(m))
			{
				m->die();
				getHit();
			}
		}
	}
}
//|||||||||||||||||||||||||||||||||||||||||||||||
//BULLET MANAGER
//|||||||||||||||||||||||||||||||||||||||||||||||
void GameState::checkShoot()
{
	Ogre::Real m_pViewportWidth = OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth();
	Ogre::Real m_pViewportHeight = OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight();

	spawnBullet( (m_shootPos.x - m_pViewportWidth / 2) / m_pViewportWidth * 2 * 30 + m_pCamera->getPosition().x
		, -(m_shootPos.y - m_pViewportHeight / 2) / m_pViewportHeight * 2 * 30+ m_pCamera->getPosition().y
);
}

void GameState::spawnBullet(int xPos, int yPos){
	//for each(Meteor* m in m_meteorList)
	Ogre::Vector3 spawnPos = Ogre::Vector3(xPos, yPos, 0);

	Ogre::Vector3 direction = m_currentLookPos-spawnPos + m_pCamera->getPosition();
	direction.x = -direction.x;
	direction.y = -direction.y;

	for (auto b:m_bulletList)
	{
		if (!b->isActive())
		{
			b->reset(xPos,yPos, direction);//Will Be Optimized  to objPool Soon
			return;
		}
	}
	Bullet *b = new Bullet();
	m_bulletList.push_back(b);
	b->setSceneManager(m_pSceneMgr);
	b->create(xPos,yPos,direction);
}

void GameState::updateBullet(double timeSinceLastFrame)
{
	for(auto b : m_bulletList)
	{
		if (b->isActive())
		{
			b->move(timeSinceLastFrame);
			for (auto m : m_meteorList)
			{
				if (m->isActive() && isIntersect(m, b))
				{
					upScore();
					m->die();
					
					b->die();
					
					break;
				}
			}
		}
	}
	//Collision Detection Will Be Implemented Here
}

void GameState::checkGenerateBullet(double timeSinceLastFrame)
{
	//Will be implement 
	//Single pulser
	if(m_bLMouseDown)
	{
		if (m_manaPoint >= m_manaPerShot)
		{
			m_bulletElapsedTime += (float)timeSinceLastFrame;
			if (m_bulletElapsedTime >= m_bulletDelay)
			{
				m_bulletElapsedTime = 0;
				checkShoot();
				m_manaPoint -= m_manaPerShot;
				m_manaPoint = m_manaPoint <= 0 ? 0 : m_manaPoint;
			}
		}
	}
	else{
		m_manaPoint += (int)(m_manaPerTime * (float)timeSinceLastFrame);
		m_manaPoint = m_manaPoint > m_maxManaPoint ? m_maxManaPoint : m_manaPoint;
	}
}

//|||||||||||||||||||||||||||||||||||||||||||||||
//GAME MANAGER
//|||||||||||||||||||||||||||||||||||||||||||||||
bool GameState::isIntersect(Meteor* m, Bullet* b)
{
	Ogre::Vector3 p1 = m->getPosition();
	Ogre::Vector3 p2 = b->getPosition();
	return abs(p1.x - p2.x) < 10 && abs(p1.y - p2.y) < 10 && abs(p1.z - p2.z) < 10;
	/*Ogre::Vector3* allCheckPoint;
	b->getPoint(allCheckPoint);
	for (size_t i = 0; i < 8; i++)
	{
		Ogre::Vector3 p = allCheckPoint[i];
		if (m->isIn(p))
			return true;
	}
	return false;*/
}

bool GameState::isCrashPlayer(Meteor *m)
{
	Ogre::Vector3 p1 = m->getPosition();
	Ogre::Vector3 p2 = m_pCamera->getPosition();
	return abs(p1.x - p2.x) < 10 && abs(p1.y - p2.y) < 10 && abs(p1.z - p2.z) < 10;
}

void GameState::upScore()
{
	int minScore = 500;
	int maxScore = 1000;
	int scoreRand = (int)(Ogre::Math::UnitRandom() * (maxScore-minScore)) + minScore;

	m_score += scoreRand;
}

void GameState::getHit()
{
	m_hitPoint -= m_damagePerHit;
	if (m_hitPoint <= 0)
	{
		m_hitPoint = 0;
		m_isAlive = false;
	}
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::buildGUI()
{
    OgreFramework::getSingletonPtr()->m_pTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    //OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_TOP, "GameLbl", "Game mode", 250);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->showCursor();

    Ogre::StringVector items;
    items.push_back("cam.pX");
    items.push_back("cam.pY");
    items.push_back("cam.pZ");
    items.push_back("cam.oW");
    items.push_back("cam.oX");
    items.push_back("cam.oY");
    items.push_back("cam.oZ");
	items.push_back("mouseX");
    items.push_back("mouseY");
	items.push_back("score");
	items.push_back("HP");
	items.push_back("MP");
    items.push_back("Mode");

    m_pDetailsPanel = OgreFramework::getSingletonPtr()->m_pTrayMgr->createParamsPanel(OgreBites::TL_TOPLEFT, "DetailsPanel", 200, items);
    m_pDetailsPanel->show();

  //  Ogre::String infoText = "Controls\n[TAB] - Switch input mode\n\n[W] - Forward / Mode up\n[S] - Backwards/ Mode down\n[A] - Left\n";
   // infoText.append("[D] - Right\n\nPress [SHIFT] to move faster\n\n[O] - Toggle FPS / logo\n");
   // infoText.append("[Print] - Take screenshot\n\n[ESC] - Exit");
   // OgreFramework::getSingletonPtr()->m_pTrayMgr->createTextBox(OgreBites::TL_RIGHT, "InfoPanel", infoText, 300, 220);

    Ogre::StringVector displayModes;
    displayModes.push_back("Solid mode");
    displayModes.push_back("Wireframe mode");
    displayModes.push_back("Point mode");
    //OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSelectMenu(OgreBites::TL_TOPRIGHT, "DisplayModeSelMenu", "Display Mode", 200, 3, displayModes);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::itemSelected(OgreBites::SelectMenu* menu)
{
    switch(menu->getSelectionIndex())
    {
    case 0:
        m_pCamera->setPolygonMode(Ogre::PM_SOLID);break;
    case 1:
        m_pCamera->setPolygonMode(Ogre::PM_WIREFRAME);break;
    case 2:
        m_pCamera->setPolygonMode(Ogre::PM_POINTS);break;
    }
}

//|||||||||||||||||||||||||||||||||||||||||||||||