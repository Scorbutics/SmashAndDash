#ifndef DEF_OBSERVER
#define DEF_OBSERVER

#include <string>
namespace ska {
	class EventArg
	{
	public:
		EventArg(){}
		virtual int type() const = 0;
		virtual ~EventArg(){}

	};

	template <class T>
	class Observable;

	template <class T>
	class Observer {
	public:
		virtual void update(Observable<T>* obs, const EventArg& e, T& t) = 0;
	};
}
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
