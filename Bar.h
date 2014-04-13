#ifndef DEF_BAR
#define DEF_BAR

#include <iostream>
#include <string>
#include <SDL2/SDL.h>

#include "Texture.h"

class SpriteBank;


//HP Bar, PP Bar, etc...
class Bar
{
    public:
    Bar(std::string barStyleName, std::string barContentName, int maxValue, SDL_Rect pos);
    Bar();

    void refresh();
    bool isVisible();

    void setMaxValue(int mv);
    void setCurrentValue(int v);
    void setBarStyle(std::string name);
    void setPos(SDL_Rect pos);
    void setBarContent(std::string name);
    void setVisible(bool x);

    int getCurrentValue();
    SDL_Rect getPos();
	~Bar();
    protected:
    Texture m_barStyle, m_barContent;
    SDL_Rect m_pos, m_barSize;
    int m_maxValue, m_curValue;
    bool m_visible;
};

#endif
