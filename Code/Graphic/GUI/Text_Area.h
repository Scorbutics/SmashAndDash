#pragma once

#include <string>
#include "Window_Area.h"
#include "DialogMenu.h"

class Text_Area : public Window_Area
{
    public:
		Text_Area(DialogMenu *parent, const std::string& text, int fontSize, ska::Rectangle relativePos);
		virtual std::string getKey(){return "";};
		virtual void forceValue(unsigned int){};
		virtual ~Text_Area();
		virtual void display();
    
	private:
		std::string m_text;
		unsigned int m_fontSize;
		ska::Texture m_stext;
		SDL_Color m_color;
};

