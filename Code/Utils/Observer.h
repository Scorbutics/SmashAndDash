#ifndef DEF_OBSERVER
#define DEF_OBSERVER

#include <string>
#include <SDL2/SDL.h>
#include "../Inputs/EventArg.h"


class Observable;
class Observer
{
	public:
		virtual void update(Observable* obs, EventArg* e) = 0;
};

/*
class MouseListener : public Observer
{
	public:
		virtual void mousePressed(MouseEvent e) = 0;
		virtual void mouseReleased(MouseEvent e) = 0;
		virtual void mousePressedState(MouseEvent e) = 0;
		virtual void mouseMoved(MouseEvent e) = 0;
		
};

class KeyListener : public Observer
{
	public:
		virtual void keyPressed(KeyEvent e) = 0;
		virtual void keyReleased(KeyEvent e) = 0;
		virtual void keyPressedState(KeyEvent e) = 0;
};
*/
#endif
