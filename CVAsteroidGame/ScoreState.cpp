//|||||||||||||||||||||||||||||||||||||||||||||||

#include "ScoreState.hpp"

//|||||||||||||||||||||||||||||||||||||||||||||||

using namespace Ogre;

//|||||||||||||||||||||||||||||||||||||||||||||||

ScoreState::ScoreState()
{
	m_bQuit = false;
	m_FrameEvent = Ogre::FrameEvent();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void ScoreState::enter()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering ScoreState...");


	m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC, "ScoreSceneMgr");

	m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

	m_pSceneMgr->addRenderQueueListener(OgreFramework::getSingletonPtr()->m_pOverlaySystem);

	m_pCamera = m_pSceneMgr->createCamera("ScoreCam");
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

	OgreFramework::getSingletonPtr()->m_pTrayMgr->createDecorWidget(OgreBites::TL_CENTER, "Game Logo", "SdkTrays/Logo2");
	Ogre::String infoText = "GAME OVER\n";
	infoText.append("\n");
	infoText.append("Score : 10000\n");

	OgreFramework::getSingletonPtr()->m_pTrayMgr->createTextBox(OgreBites::TL_CENTER, "ScorePanel", infoText, 300, 75);
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_CENTER, "BackBtn", " Back to Main Menu ", 250);

	createScene();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void ScoreState::createScene()
{
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void ScoreState::exit()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving ScoreState...");

	m_pSceneMgr->destroyCamera(m_pCamera);
	if (m_pSceneMgr)
		OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);

	OgreFramework::getSingletonPtr()->m_pTrayMgr->clearAllTrays();
	OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyAllWidgets();
	OgreFramework::getSingletonPtr()->m_pTrayMgr->setListener(0);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool ScoreState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	//Press Escape at this menu to quit
	if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		m_bQuit = true;
		return true;
	}

	OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool ScoreState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
	OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool ScoreState::mouseMoved(const OIS::MouseEvent &evt)
{
	if (OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseMove(evt)) return true;
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool ScoreState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if (OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseDown(evt, id)) return true;
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool ScoreState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if (OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseUp(evt, id)) return true;
	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void ScoreState::update(double timeSinceLastFrame)
{
	m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
	OgreFramework::getSingletonPtr()->m_pTrayMgr->frameRenderingQueued(m_FrameEvent);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void ScoreState::buttonHit(OgreBites::Button *button)
{
	if (button->getName() == "BackBtn")
		changeAppState(findByName("MenuState"));
}

//|||||||||||||||||||||||||||||||||||||||||||||||