#pragma once
#include <iostream>
#include <string>

#include "DynamicWindowArea.h"
#include "../../ska/Graphic/Texture.h"
#include "../../ska/Graphic/Rectangle.h"

namespace ska {
	class InputContextManager;
}

class Button_Quit : public DynamicWindowArea
{
public:
	Button_Quit(DialogMenu &parent, const ska::InputContextManager& icm, const std::string& imgName, const std::string& secondImgName, ska::Rectangle relativePos);
    std::string getAction();
	void setAction(const std::string& action);
	virtual void refresh() override;
	virtual void display() const override;
    virtual void forceValue(unsigned int){};
    virtual std::string getKey(){return "";};
    virtual ~Button_Quit();

private:
	const ska::InputContextManager& m_playerICM;
    ska::Texture m_sprite, m_spriteActive;
    int m_lastMouseState;
};
