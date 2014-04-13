#ifndef DEF_WINDOW_CHOICE
#define DEF_WINDOW_CHOICE

#include "Button.h"
#include "WGameCore.h"

class Window_Choice : public Window_Area
{
    public:
    Window_Choice(DialogMenu* parent);
    virtual void refresh();
    virtual std::string getKey(){return "";};
    virtual void forceValue(unsigned int){};
    virtual ~Window_Choice();

    private:
    vector<unique_ptr<Button>> m_buttons;
};

#endif
