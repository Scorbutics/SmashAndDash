#include <iostream>
#include <fstream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "Text_Area.h"
#include "../../Utils/IDs.h"
using namespace std;

Text_Area::Text_Area(DialogMenu *parent, string text, int fontSize, SDL_Rect relativePos): Window_Area(parent)
{
    m_type = BUTTON_TEXT_AREA;
    m_text = text;
    m_active = parent->isVisible();
    m_relativePos.x = relativePos.x;
    m_relativePos.y = relativePos.y;
    m_fontSize = fontSize;

    m_color.r = 0;
    m_color.g = 0;
    m_color.b = 0;
	m_color.a = 255;

    m_stext.loadFromText(m_fontSize, m_text, m_color);

    m_relativePos.w = m_stext.getWidth();
    m_relativePos.h = m_stext.getHeight();
}

void Text_Area::display()
{

    if(!m_parent->isVisible())
        return;
    else
        m_active = true;

    SDL_Rect buf = m_relativePos;
    buf.x += (m_parent->getPos())->x;
    buf.y += (m_parent->getPos())->y;
	
	m_stext.render(buf.x, buf.y);

}

Text_Area::~Text_Area()
{
}
