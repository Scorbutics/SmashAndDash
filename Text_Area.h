#ifndef DEF_TEXT_AREA
#define DEF_TEXT_AREA

#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Texture.h"
#include "Window_Area.h"

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
		int m_fontSize;
		TTF_Font *m_font;
		Texture m_stext;
		SDL_Color m_color;
};

#endif
