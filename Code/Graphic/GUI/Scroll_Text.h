#pragma once

#include <string>
#include <vector>
#include "../../ska/Graphic/Rectangle.h"
#include "DynamicWindowArea.h"
#include "../../ska/Graphic/Texture.h"

class Scroll_Text : public DynamicWindowArea
{
public:
	Scroll_Text(DialogMenu *parent, const std::string& buttonAspect, int height, int width, const std::vector<std::string>& text, int fontSize, ska::Rectangle relativePos);
    virtual void display();
    virtual std::string getKey(){return "";};
    virtual void forceValue(unsigned int){};
    virtual ~Scroll_Text();
	virtual void refresh();

private:
    std::vector<std::string> m_text;
    unsigned int m_fontSize, m_linesNumber, m_width;
    int m_lastMouseState, m_start;
    std::vector<ska::Texture> m_stext;
    ska::Texture m_topArrow, m_botArrow, m_cursor, m_scrollBar;
	ska::Rectangle m_posTopArrow, m_posBotArrow, m_posCursor;
    ska::Color m_color;
};
