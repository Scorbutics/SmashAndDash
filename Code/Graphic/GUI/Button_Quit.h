#ifndef DEF_BUTTON_QUIT
#define DEF_BUTTON_QUIT

#include <iostream>
#include <string>

#include "DynamicWindowArea.h"
#include "../../ska/Graphic/Texture.h"
#include "../../ska/Graphic/Rectangle.h"

using namespace std;

class Button_Quit : public DynamicWindowArea
{
    public:
		Button_Quit(DialogMenu *parent, string imgName, string secondImgName, ska::Rectangle relativePos);
    string getAction();
    void setAction(string action);
    virtual void refresh();
	virtual void display();
    virtual void forceValue(unsigned int){};
    virtual std::string getKey(){return "";};
    virtual ~Button_Quit();

    private:
    ska::Texture m_sprite, m_spriteActive;
    int m_lastMouseState;
};

#endif
