#include <iostream>
#include <fstream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "IDs.h"
#include "Text_Area.h"
#include "DialogMenu.h"
#include "ChargementImages.h"
using namespace std;

Text_Area::Text_Area(DialogMenu *parent, string text, int fontSize, SDL_Rect relativePos): Window_Area(parent)
{
    m_type = BUTTON_TEXT_AREA;
    m_text = text;
    m_active = parent->isVisible();
    m_relativePos.x = relativePos.x;
    m_relativePos.y = relativePos.y;
    m_fontSize = fontSize;
    m_font = TTF_OpenFont(FILE_FONT, m_fontSize);
    if(m_font == NULL)
        cerr << "Erreur lors de l'ouverture de la police : " << TTF_GetError() << endl;

    m_color.r = 0;
    m_color.g = 0;
    m_color.b = 0;
	m_color.a = 255;

    m_stext.loadFromText(m_font, m_text, m_color);

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
    //TTF_CloseFont(m_font);
}
