//|||||||||||||||||||||||||||||||||||||||||||||||

#include "GameState.hpp"
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

	m_move				= false;

    m_pDetailsPanel		= 0;
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
	m_pCamera->setPosition(Ogre::Vector3(0, 0, 20));
	m_pCamera->lookAt(Ogre::Vector3(0, 0, -20));
    m_pCamera->setNearClipDistance(5);

	m_pCamera->setAspectRatio(Ogre::Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth()) /
		Ogre::Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight()));

    OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
    m_pCurrentObject = 0;

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

	m_pSceneMgr->getEntity("Cube01")->setQueryFlags(CUBE_MASK);
    m_pSceneMgr->getEntity("Cube02")->setQueryFlags(CUBE_MASK);
    m_pSceneMgr->getEntity("Cube03")->setQueryFlags(CUBE_MASK);

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

	cube = m_pSceneMgr->createEntity("Cube01.mesh");

	cube->setQueryFlags(CUBE_MASK);

	cubeNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(
		Ogre::Vector3(0, 0, -200.0));
	cubeNode->attachObject(cube);

	m_pSceneMgr->setSkyBox(true, "SkyBox/Space");
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

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseDown(evt, id)) return true;

    if(id == OIS::MB_Left)
    {
        onLeftPressed(evt);
        m_bLMouseDown = true;
		m_move = true;
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
		m_move = false;
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
    }
}
//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::moveByHeadPose()
{
	
	float* rotMat = (CVProcess::getInstance().mHeadPose)->getHeadRotationMatrix();
	Ogre::Matrix3 currentRotationMatrix(
		rotMat[0], rotMat[1], rotMat[2],
		rotMat[3], rotMat[4], rotMat[5],
		rotMat[6], rotMat[7], rotMat[8]);

	Ogre::Radian euler_x, euler_y, euler_z;
	currentRotationMatrix.ToEulerAnglesXYZ(euler_x, euler_y, euler_z);

	//m_pCamera->yaw(euler_x);
	//m_pCamera->pitch(euler_y);

	/*Ogre::Quaternion cur_Q = Ogre::Quaternion(currentRotationMatrix);
	Ogre::Quaternion last_Q = Ogre::Quaternion(m_LastHeadPose);

	Ogre::Radian rel_pitch = cur_Q.getPitch() - last_Q.getPitch();
	Ogre::Radian rel_yaw = cur_Q.getYaw() - last_Q.getYaw();*/

	m_pCamera->setOrientation(Ogre::Quaternion(currentRotationMatrix));

	m_LastHeadPose = currentRotationMatrix;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::update(double timeSinceLastFrame)
{
    m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    OgreFramework::getSingletonPtr()->m_pTrayMgr->frameRenderingQueued(m_FrameEvent);

    if(m_bQuit == true)
    {
        popAppState();
        return;
    }

	Ogre::Vector3* dist = new Ogre::Vector3(1, 0, 0);
	// FIX_HERE
	//if (m_move)
	{
		cubeNode->translate(Ogre::Vector3(0, 0, 1));
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
            if(m_bSettingsMode)
                m_pDetailsPanel->setParamValue(7, "Buffered Input");
            else
                m_pDetailsPanel->setParamValue(7, "Un-Buffered Input");
        }
    }

    m_MoveScale = m_MoveSpeed   * timeSinceLastFrame;
    m_RotScale  = m_RotateSpeed * timeSinceLastFrame;

	m_TranslateVector = Ogre::Vector3::ZERO;

    getInput();
    moveCamera();
	// Move camera by head pose
	moveByHeadPose();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::buildGUI()
{
    OgreFramework::getSingletonPtr()->m_pTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_TOP, "GameLbl", "Game mode", 250);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->showCursor();

    Ogre::StringVector items;
    items.push_back("cam.pX");
    items.push_back("cam.pY");
    items.push_back("cam.pZ");
    items.push_back("cam.oW");
    items.push_back("cam.oX");
    items.push_back("cam.oY");
    items.push_back("cam.oZ");
    items.push_back("Mode");

    m_pDetailsPanel = OgreFramework::getSingletonPtr()->m_pTrayMgr->createParamsPanel(OgreBites::TL_TOPLEFT, "DetailsPanel", 200, items);
    m_pDetailsPanel->show();

    Ogre::String infoText = "Controls\n[TAB] - Switch input mode\n\n[W] - Forward / Mode up\n[S] - Backwards/ Mode down\n[A] - Left\n";
    infoText.append("[D] - Right\n\nPress [SHIFT] to move faster\n\n[O] - Toggle FPS / logo\n");
    infoText.append("[Print] - Take screenshot\n\n[ESC] - Exit");
    OgreFramework::getSingletonPtr()->m_pTrayMgr->createTextBox(OgreBites::TL_RIGHT, "InfoPanel", infoText, 300, 220);

    Ogre::StringVector displayModes;
    displayModes.push_back("Solid mode");
    displayModes.push_back("Wireframe mode");
    displayModes.push_back("Point mode");
    OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSelectMenu(OgreBites::TL_TOPRIGHT, "DisplayModeSelMenu", "Display Mode", 200, 3, displayModes);
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