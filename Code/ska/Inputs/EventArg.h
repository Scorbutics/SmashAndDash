#ifndef DEF_EVENTARG
#define DEF_EVENTARG

#include <string>
#include "../Utils\IDs.h"

class EventArg
{
	public:
		EventArg(){}
		virtual int type() = 0;
		virtual ~EventArg(){}

};


namespace ska {
	class MouseEvent : public EventArg
	{
	public:
		MouseEvent() : m_state(0), m_button(-1), m_x(-1), m_y(-1){}
		virtual int type(){ return EVENT_MOUSE; }
		int getButton() const { return m_button; }
		void setButton(int b){ m_button = b; }
		void setState(int s) { m_state = s; }
		int getState() const { return m_state; }
		int getX() const { return m_x; }
		int getY() const { return m_y; }
		void setX(int x) { m_x = x; }
		void setY(int y) { m_y = y; }
		bool clicked() const { return m_click; }
		void setClicked(bool b) { m_click = b; }
		virtual ~MouseEvent(){}

	private:
		int m_button;
		bool m_click;
		int m_state;
		int m_x, m_y;
	};

	class KeyEvent : public EventArg
	{
	public:
		KeyEvent() : m_state(0), m_pressed(false), m_scancode(-1), m_secondScancode(-1) {}
		virtual int type(){ return EVENT_KEYBOARD; }
		int getScancode() const { return m_scancode; }
		void setScancode(int s){ m_scancode = s; }
		int getSecondScancode() const { return m_secondScancode; }
		void setSecondScancode(int s){ m_secondScancode = s; }
		void setState(int s) { m_state = s; }
		int getState() const { return m_state; }
		bool pressed() const { return m_pressed; }
		void setPressed(bool b) { m_pressed = b; }
		virtual ~KeyEvent(){}

	private:
		int m_scancode, m_secondScancode;
		bool m_pressed;
		int m_state;
	};
}
#endif
