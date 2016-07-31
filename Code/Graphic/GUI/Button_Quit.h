#ifndef DEF_BUTTON_QUIT
#define DEF_BUTTON_QUIT

#include <iostream>
#include <string>

#include "DynamicWindowArea.h"
#include "../../ska/Graphic/Texture.h"
#include "../../ska/Graphic/Rectangle.h"

class Button_Quit : public DynamicWindowArea
{
public:
	Button_Quit(DialogMenu *parent, const std::string& imgName, const std::string& secondImgName, ska::Rectangle relativePos);
    std::string getAction();
	void setAction(const std::string& action);
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
