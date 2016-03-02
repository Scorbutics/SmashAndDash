#include "MouseInput.h"

ska::MouseInput::MouseInput()
{
	m_mousex = 0;
	m_mousey = 0;
	m_mousexrel = 0;
	m_mouseyrel = 0;
	m_clickPos.x = m_clickPos.y = 0;
	this->resetAll();
}


ska::MouseInput::~MouseInput()
{
}


ska::Rectangle ska::MouseInput::getMouseLastPos()
{
	return m_mouseLastPos;
}



void ska::MouseInput::setMouseState(int touche, int state)
{
	
	m_mousebutton[touche] = state;
	m_mouseEvent.setButton(touche);
	m_mouseEvent.setState(state);
	m_mouseEvent.setClicked(false);

	if (this->mouseClick(touche))
	{
		m_mouseEvent.setClicked(true);
		m_clickPos.x = m_mousex;
		m_clickPos.y = m_mousey;
	}

	//notifyObservers(&m_mouseEvent);
}

void ska::MouseInput::setMouseLastState(int touche, int x)
{
	m_mouseLastState[touche] = x;
}


int ska::MouseInput::getMouseState(int touche)
{
	return m_mousebutton[touche];
}

int ska::MouseInput::getMouseLastState(int touche)
{
	return m_mouseLastState[touche];
}


void ska::MouseInput::setMousePos(SDL_Event event)
{
	m_mousex = event.motion.x;
	m_mousey = event.motion.y;
	m_mousexrel = event.motion.xrel;
	m_mouseyrel = event.motion.yrel;
	
	m_mouseEvent.setX(event.motion.x);
	m_mouseEvent.setY(event.motion.y);

	//notifyObservers(&m_mouseEvent);
}

ska::Rectangle ska::MouseInput::getMousePos()
{
	ska::Rectangle buf;

	buf.x = m_mousex;
	buf.y = m_mousey;
	buf.h = 0;
	buf.w = 0;

	return buf;
}

void ska::MouseInput::resetAll()
{
	if (m_mousebutton[SDL_BUTTON_RIGHT] != 0)
		m_mousebutton[SDL_BUTTON_RIGHT] = 0;
	if (m_mousebutton[SDL_BUTTON_LEFT] != 0)
		m_mousebutton[SDL_BUTTON_LEFT] = 0;
	if (m_mouseLastState[SDL_BUTTON_RIGHT] != 0)
		m_mouseLastState[SDL_BUTTON_RIGHT] = 0;
	if (m_mouseLastState[SDL_BUTTON_LEFT] != 0)
		m_mouseLastState[SDL_BUTTON_LEFT] = 0;

}


bool ska::MouseInput::mouseClick(int touche)
{
	if(m_mousebutton[touche] == 1 && m_mouseLastState[touche] == 0)
	{
		m_mouseEvent.setButton(touche);
		m_mouseEvent.setClicked(true);
		//notifyObservers(&m_mouseEvent);
		return true;
	}

	return false;
}

ska::Rectangle ska::MouseInput::getMouseClickPos()
{
	return m_clickPos;
}

ska::Rectangle ska::MouseInput::getMouseTranslation()
{
	ska::Rectangle mouseTrans;
	mouseTrans.x = m_mousex - m_mouseLastPos.x;
	mouseTrans.y = m_mousey - m_mouseLastPos.y;
	return mouseTrans;
}

void ska::MouseInput::setMouseLastPos(ska::Rectangle mouselastpos)
{
	m_mouseLastPos = mouselastpos;
}
