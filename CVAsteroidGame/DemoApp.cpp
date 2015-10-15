//|||||||||||||||||||||||||||||||||||||||||||||||

#include "DemoApp.hpp"

#include "MenuState.hpp"
#include "GameState.hpp"
#include "PauseState.hpp"

//|||||||||||||||||||||||||||||||||||||||||||||||

DemoApp::DemoApp()
{
	m_pAppStateManager = 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

DemoApp::~DemoApp()
{
	delete m_pAppStateManager;
	delete m_CVProcess;
    delete OgreFramework::getSingletonPtr();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void DemoApp::startDemo()
{
	new OgreFramework();
	if(!OgreFramework::getSingletonPtr()->initOgre("AdvancedOgreFramework", 0, 0))
		return;

	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Demo initialized!");

	m_pAppStateManager = new AppStateManager();

	// Init CV process
	m_CVProcess = new CVProcess();
	m_CVProcess->init();


	// Create scenes
	MenuState::create(m_pAppStateManager, "MenuState");
	GameState::create(m_pAppStateManager, "GameState");
    PauseState::create(m_pAppStateManager, "PauseState");

	// Start!
	m_pAppStateManager->start(m_pAppStateManager->findByName("MenuState"));

	
}

//|||||||||||||||||||||||||||||||||||||||||||||||