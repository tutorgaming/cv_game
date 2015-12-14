//|||||||||||||||||||||||||||||||||||||||||||||||

#include "MenuState.hpp"
#include "CVProcess.hpp"


//|||||||||||||||||||||||||||||||||||||||||||||||

//|||||||||||||||||||||||||||||||||||||||||||||||

MenuState::MenuState()
{
    m_bQuit         = false;
    m_FrameEvent    = Ogre::FrameEvent();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void MenuState::enter()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering MenuState...");


	m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(Ogre::ST_GENERIC, "MenuSceneMgr");

    m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

    m_pSceneMgr->addRenderQueueListener(OgreFramework::getSingletonPtr()->m_pOverlaySystem);
	
    m_pCamera = m_pSceneMgr->createCamera("MenuCam");
	m_pCamera->setPosition(Ogre::Vector3(0, 25, -50));
	m_pCamera->lookAt(Ogre::Vector3(0, 0, 0));
    m_pCamera->setNearClipDistance(1);

	m_pCamera->setAspectRatio(Ogre::Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth()) /
		Ogre::Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight()));

	OgreFramework::getSingletonPtr()->m_pTrayMgr->hideBackdrop();

	OgreFramework::getSingletonPtr()->m_pViewport->setBackgroundColour(Ogre::ColourValue(0.00f, 0.00f, 0.00f, 1.0f));
    OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

    OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgets();
    OgreFramework::getSingletonPtr()->m_pTrayMgr->showCursor();

	OgreFramework::getSingletonPtr()->m_pTrayMgr->createDecorWidget(OgreBites::TL_CENTER,"Game Logo","SdkTrays/Logo2");
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createSeparator(OgreBites::TL_CENTER,"Seperator");
    OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_CENTER, "EnterBtn", " Play Now ! ", 250);
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_CENTER, "CreditBtn", " Credit ", 250);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_CENTER, "ExitBtn", " Exit ", 250);
	
    createScene();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void MenuState::createScene()
{
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void MenuState::exit()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving MenuState...");

    m_pSceneMgr->destroyCamera(m_pCamera);
    if(m_pSceneMgr)
        OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);

    OgreFramework::getSingletonPtr()->m_pTrayMgr->clearAllTrays();
    OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgets();
    OgreFramework::getSingletonPtr()->m_pTrayMgr->setListener(0);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool MenuState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	//Press Escape at this menu to quit
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
    {
        m_bQuit = true;
        return true;
    }

	//Press O to select templates or recalibrate tracking
	if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_O))
	{
		CVProcess::getInstance().mHeadPose->selectTemplates();
		return true;
	}

	//Press R to re-detect facial features
	if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_R))
	{
		CVProcess::getInstance().mHeadPose->calibrateDetection();
		return true;
	}

    OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool MenuState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool MenuState::mouseMoved(const OIS::MouseEvent &evt)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseMove(evt)) return true;
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool MenuState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseDown(evt, id)) return true;
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool MenuState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseUp(evt, id)) return true;
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void MenuState::update(double timeSinceLastFrame)
{
    m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    OgreFramework::getSingletonPtr()->m_pTrayMgr->frameRenderingQueued(m_FrameEvent);

    if(m_bQuit == true)
    {
        shutdown();
        return;
    }
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void MenuState::buttonHit(OgreBites::Button *button)
{
    if(button->getName() == "ExitBtn")
        m_bQuit = true;
    else if(button->getName() == "EnterBtn")
        changeAppState(findByName("GameState"));
	else if(button->getName() == "CreditBtn")
        changeAppState(findByName("CreditState"));
}

//|||||||||||||||||||||||||||||||||||||||||||||||