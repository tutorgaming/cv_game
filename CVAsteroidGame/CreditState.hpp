//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef CREDIT_STATE_HPP
#define CREDIT_STATE_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AppState.hpp"

//|||||||||||||||||||||||||||||||||||||||||||||||

class CreditState : public AppState
{
public:
    CreditState();

	DECLARE_APPSTATE_CLASS(CreditState)

	void enter();
	void createScene();
	void exit();

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &evt);
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

	void buttonHit(OgreBites::Button* button);

	void update(double timeSinceLastFrame);

private:
	bool                        m_bQuit;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||