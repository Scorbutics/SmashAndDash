#ifndef DEF_TEXT_AREA
#define DEF_TEXT_AREA

#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "Window_Area.h"
#include "DialogMenu.h"

using namespace std;

class Text_Area : public Window_Area
{
    public:
		Text_Area(DialogMenu *parent, string text, int fontSize, SDL_Rect relativePos);
		virtual std::string getKey(){return "";};
		virtual void forceValue(unsigned int){};
		virtual ~Text_Area();
		virtual void display();
    
	private:
		string m_text;
		unsigned int m_fontSize;
		Texture m_stext;
		SDL_Color m_color;
};

#endif
