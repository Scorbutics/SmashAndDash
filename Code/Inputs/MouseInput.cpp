#include "MouseInput.h"

MouseInput::MouseInput()
{
	m_mousex = 0;
	m_mousey = 0;
	m_mousexrel = 0;
	m_mouseyrel = 0;
	m_clickPos.x = m_clickPos.y = 0;
	this->resetAll();
}


MouseInput::~MouseInput()
{
}


SDL_Rect MouseInput::getMouseLastPos()
{
	return m_mouseLastPos;
}



void MouseInput::setMouseState(int touche, int state)
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

void MouseInput::setMouseLastState(int touche, int x)
{
	m_mouseLastState[touche] = x;
}


int MouseInput::getMouseState(int touche)
{
	return m_mousebutton[touche];
}

int MouseInput::getMouseLastState(int touche)
{
	return m_mouseLastState[touche];
}


void MouseInput::setMousePos(SDL_Event event)
{
	m_mousex = event.motion.x;
	m_mousey = event.motion.y;
	m_mousexrel = event.motion.xrel;
	m_mouseyrel = event.motion.yrel;
	
	m_mouseEvent.setX(event.motion.x);
	m_mouseEvent.setY(event.motion.y);

	//notifyObservers(&m_mouseEvent);
}

SDL_Rect MouseInput::getMousePos()
{
	SDL_Rect buf;

	buf.x = m_mousex;
	buf.y = m_mousey;
	buf.h = 0;
	buf.w = 0;

	return buf;
}

void MouseInput::resetAll()
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


bool MouseInput::mouseClick(int touche)
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

SDL_Rect MouseInput::getMouseClickPos()
{
	return m_clickPos;
}

SDL_Rect MouseInput::getMouseTranslation()
{
	SDL_Rect mouseTrans;
	mouseTrans.x = m_mousex - m_mouseLastPos.x;
	mouseTrans.y = m_mousey - m_mouseLastPos.y;
	return mouseTrans;
}

void MouseInput::setMouseLastPos(SDL_Rect mouselastpos)
{
	m_mouseLastPos = mouselastpos;
}
