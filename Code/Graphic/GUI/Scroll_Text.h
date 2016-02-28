#ifndef DEF_SCROLL_TEXT
#define DEF_SCROLL_TEXT

#include <iostream>
#include <string>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include "../Rectangle.h"
#include "DynamicWindowArea.h"
#include "../Texture.h"

using namespace std;

class Scroll_Text : public DynamicWindowArea
{
    public:
		Scroll_Text(DialogMenu *parent, string buttonAspect, int height, int width, vector<string> text, int fontSize, ska::Rectangle relativePos);
    virtual void display();
    virtual std::string getKey(){return "";};
    virtual void forceValue(unsigned int){};
    virtual ~Scroll_Text();
	virtual void refresh();
    private:
    vector<string> m_text;
    unsigned int m_fontSize, m_linesNumber, m_width;
    int m_lastMouseState, m_start;
    vector<Texture> m_stext;
    Texture m_topArrow, m_botArrow, m_cursor, m_scrollBar;
	ska::Rectangle m_posTopArrow, m_posBotArrow, m_posCursor;
    SDL_Color m_color;
};

#endif
