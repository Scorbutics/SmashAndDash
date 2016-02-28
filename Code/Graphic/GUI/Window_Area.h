#ifndef DEF_WINDOW_AREA
#define DEF_WINDOW_AREA

#include <iostream>
#include <string>
#include "../Rectangle.h"

class DialogMenu;
class WGameCore;

class Window_Area
{
    public:
    Window_Area(DialogMenu* parent);
    bool canBeClicked();
    bool isVisible();
    void hide(bool x);
	ska::Rectangle* getRelativePos();
	void setRelativePos(ska::Rectangle pos);
    DialogMenu* getParent();
	virtual void display() = 0;
    virtual std::string getKey() = 0;
    virtual void forceValue(unsigned int index) = 0;
    virtual ~Window_Area();
	ska::Rectangle getAbsolutePos();
    bool isA(int type);

    protected:
    DialogMenu* m_parent;
	ska::Rectangle m_relativePos;
    bool m_canBeClicked, m_active;
    int m_type;

};

#endif
