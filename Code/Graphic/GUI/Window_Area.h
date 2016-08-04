#pragma once

#include <iostream>
#include <string>
#include "../../ska/Graphic/Point.h"

class DialogMenu;
class WGameCore;

class Window_Area
{
public:
    Window_Area(DialogMenu* parent);
	virtual ~Window_Area();

	bool canBeClicked();
    bool isVisible();
    void hide(bool x);
	ska::Point<int>& getRelativePos();
	void setRelativePos(ska::Point<int> pos);
    DialogMenu* getParent();
	ska::Point<int> getAbsolutePos();
    bool isA(int type);

	virtual void display() = 0;
	virtual std::string getKey() = 0;
	virtual void forceValue(unsigned int index) = 0;

protected:
    DialogMenu* m_parent;
	ska::Point<int> m_relativePos;
    bool m_canBeClicked, m_active;
    int m_type;

};
