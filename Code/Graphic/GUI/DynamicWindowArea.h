#ifndef DEF_DYNAMIC_WINDOW_AREA
#define DEF_DYNAMIC_WINDOW_AREA

#include "Window_Area.h"

class DialogMenu;

class DynamicWindowArea: public Window_Area
{
public:
	DynamicWindowArea(DialogMenu* parent);
	virtual ~DynamicWindowArea();
	virtual void refresh() = 0;

};

#endif