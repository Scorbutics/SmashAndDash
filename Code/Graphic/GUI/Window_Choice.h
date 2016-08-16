#pragma once
#include "DialogMenu.h"
#include "Window_Area.h"
#include "Button.h"



class Window_Choice : public Window_Area
{
public:
    Window_Choice(DialogMenu& parent);
    virtual void refresh();
    virtual std::string getKey(){return "";};
    virtual void forceValue(unsigned int){};
    virtual ~Window_Choice();

    private:
		std::vector<std::unique_ptr<Button>> m_buttons;
};

