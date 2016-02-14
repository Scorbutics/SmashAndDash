#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include "Bar.h"
#include "Utils/IDs.h"

using namespace std;

Bar::Bar(string barStyleName, string barContentName, int maxValue, SDL_Rect pos) : m_barStyle(barStyleName, T_RED, T_GREEN, T_BLUE),
m_barContent(barContentName, T_RED, T_GREEN, T_BLUE)
{
    m_maxValue = maxValue;
    m_pos = pos;
        
    m_barSize.w = (int)((m_curValue * ((m_barStyle.getWidth() * 5) /6.)) / m_maxValue);
    m_barSize.h = (int)((m_barStyle.getHeight() * 5) /6);
    m_barSize.x = pos.x + m_barStyle.getWidth()/6 ;
    m_barSize.y = pos.y + m_barStyle.getHeight()/6;
    

    m_visible = true;
}

Bar::Bar()
{
    m_maxValue = 1;
    m_visible = true;
}

void Bar::refresh()
{
	if(!m_visible)
		return;

    SDL_Rect rectSize;
    rectSize.w = m_barSize.w;
    rectSize.h = m_barSize.h;
    rectSize.x = 0;
    rectSize.y = 0;

	
	m_barStyle.render(m_pos.x, m_pos.y);
	m_barContent.render(m_barSize.x , m_barSize.y, &rectSize);
	
}

void Bar::setPos(SDL_Rect pos)
{
    m_pos = pos;

    m_barSize.x = pos.x + m_barStyle.getWidth()/12;
    m_barSize.y = pos.y + m_barStyle.getHeight()*2/8;
    m_pos.w = m_barStyle.getWidth();
    m_pos.h = m_barStyle.getHeight();
    
}

void Bar::setMaxValue(int mv)
{
    if(mv != 0)
        m_maxValue = mv;

    
	m_barSize.w = (int)((m_curValue * ((m_barStyle.getWidth() * 5) /6.)) / m_maxValue);
}

void Bar::setCurrentValue(int v)
{
    m_curValue = v;

    m_barSize.w = (int)((m_curValue * ((m_barStyle.getWidth() * 5) /6.)) / m_maxValue);
}

void Bar::setBarStyle(string name)
{
   
    m_barStyle.load(name, T_RED, T_GREEN, T_BLUE);

    m_barSize.w = (int)((m_curValue * ((m_barStyle.getWidth() * 5) /6.)) / m_maxValue);
    m_barSize.h = (int)(((m_barStyle.getHeight() * 3) /6));
}

void Bar::setBarContent(string name)
{
	m_barContent.load(name, T_RED, T_GREEN, T_BLUE);
}

bool Bar::isVisible()
{
    return m_visible;
}

void Bar::setVisible(bool x)
{
    m_visible = x;
}

int Bar::getCurrentValue()
{
    return m_curValue;
}

SDL_Rect Bar::getPos()
{
    return m_pos;
}

Bar::~Bar()
{
}