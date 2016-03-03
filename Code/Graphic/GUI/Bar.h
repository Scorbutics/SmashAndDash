#ifndef DEF_BAR
#define DEF_BAR

#include <iostream>
#include <string>
#include "../../ska/Graphic/Texture.h"

class SpriteBank;


//HP Bar, PP Bar, etc...
class Bar
{
    public:
		Bar(std::string barStyleName, std::string barContentName, int maxValue, ska::Rectangle pos);
    Bar();

    void refresh();
    bool isVisible();

    void setMaxValue(int mv);
    void setCurrentValue(int v);
    void setBarStyle(std::string name);
	void setPos(ska::Rectangle pos);
    void setBarContent(std::string name);
    void setVisible(bool x);

    int getCurrentValue();
	ska::Rectangle getPos();
	~Bar();
    protected:
		ska::Texture m_barStyle, m_barContent;
	ska::Rectangle m_pos, m_barSize;
    int m_maxValue, m_curValue;
    bool m_visible;
};

#endif
