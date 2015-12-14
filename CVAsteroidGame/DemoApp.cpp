//|||||||||||||||||||||||||||||||||||||||||||||||

#include "DemoApp.hpp"

#include "MenuState.hpp"
#include "GameState.hpp"
#include "PauseState.hpp"
#include "CreditState.hpp"
#include "ScoreState.hpp"

//|||||||||||||||||||||||||||||||||||||||||||||||

DemoApp::DemoApp()
{
	m_pAppStateManager = 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

DemoApp::~DemoApp()
{
	delete m_pAppStateManager;
	//delete CVProcess::getSingletonPtr();
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
	// new CVProcess();
	CVProcess::getInstance().init();


	// Create scenes
	MenuState::create(m_pAppStateManager, "MenuState");
	CreditState::create(m_pAppStateManager, "CreditState");
	GameState::create(m_pAppStateManager, "GameState");
    PauseState::create(m_pAppStateManager, "PauseState");
	ScoreState::create(m_pAppStateManager, "ScoreState");

	// Start!

	m_pAppStateManager->start(m_pAppStateManager->findByName("MenuState"));

	
}

//|||||||||||||||||||||||||||||||||||||||||||||||