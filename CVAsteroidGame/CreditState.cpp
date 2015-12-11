//|||||||||||||||||||||||||||||||||||||||||||||||

#include "CreditState.hpp"

//|||||||||||||||||||||||||||||||||||||||||||||||

using namespace Ogre;

//|||||||||||||||||||||||||||||||||||||||||||||||

CreditState::CreditState()
{
    m_bQuit         = false;
    m_FrameEvent    = Ogre::FrameEvent();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void CreditState::enter()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering CreditState...");


    m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC, "CreditSceneMgr");

    m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

    m_pSceneMgr->addRenderQueueListener(OgreFramework::getSingletonPtr()->m_pOverlaySystem);
	
    m_pCamera = m_pSceneMgr->createCamera("CreditCam");
    m_pCamera->setPosition(Vector3(0, 25, -50));
    m_pCamera->lookAt(Vector3(0, 0, 0));
    m_pCamera->setNearClipDistance(1);

    m_pCamera->setAspectRatio(Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth()) /
        Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight()));

	OgreFramework::getSingletonPtr()->m_pTrayMgr->hideBackdrop();

	OgreFramework::getSingletonPtr()->m_pViewport->setBackgroundColour(ColourValue(0.00f, 0.00f, 0.00f, 1.0f));
    OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

    OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgets();
    OgreFramework::getSingletonPtr()->m_pTrayMgr->showCursor();

	OgreFramework::getSingletonPtr()->m_pTrayMgr->createDecorWidget(OgreBites::TL_CENTER,"Game Logo","SdkTrays/Logo2");
	Ogre::String infoText = "Teams\n";
	infoText.append("\n");
    infoText.append("Archan Soonthornarom\n");
	infoText.append("Pongsatorn Santiwatanakul\n");
	infoText.append("Badin Jitsuksamran\n");
	infoText.append("Theppasith Nisitsukcharoen\n");

    OgreFramework::getSingletonPtr()->m_pTrayMgr->createTextBox(OgreBites::TL_CENTER, "TeamMemberPanel", infoText, 300, 125);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_CENTER, "BackBtn", " Back to Main Menu ", 250);
	
    createScene();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void CreditState::createScene()
{
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void CreditState::exit()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving CreditState...");

    m_pSceneMgr->destroyCamera(m_pCamera);
    if(m_pSceneMgr)
        OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);

    OgreFramework::getSingletonPtr()->m_pTrayMgr->clearAllTrays();
    OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgets();
    OgreFramework::getSingletonPtr()->m_pTrayMgr->setListener(0);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool CreditState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	//Press Escape at this menu to quit
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
    {
        m_bQuit = true;
        return true;
    }

    OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool CreditState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool CreditState::mouseMoved(const OIS::MouseEvent &evt)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseMove(evt)) return true;
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool CreditState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseDown(evt, id)) return true;
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool CreditState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseUp(evt, id)) return true;
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void CreditState::update(double timeSinceLastFrame)
{
    m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    OgreFramework::getSingletonPtr()->m_pTrayMgr->frameRenderingQueued(m_FrameEvent);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void CreditState::buttonHit(OgreBites::Button *button)
{
		if(button->getName() == "BackBtn")
        changeAppState(findByName("MenuState"));
}

//|||||||||||||||||||||||||||||||||||||||||||||||