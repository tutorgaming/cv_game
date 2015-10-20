//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef OGRE_DEMO_HPP
#define OGRE_DEMO_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AdvancedOgreFramework.hpp"
#include "AppStateManager.hpp"
#include "CVProcess.hpp"

//|||||||||||||||||||||||||||||||||||||||||||||||

class DemoApp
{
public:
	DemoApp();
	~DemoApp();

	void startDemo();

private:
	AppStateManager*	m_pAppStateManager;
};


	

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||