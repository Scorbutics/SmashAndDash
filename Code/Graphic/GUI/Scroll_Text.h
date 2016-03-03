#ifndef DEF_SCROLL_TEXT
#define DEF_SCROLL_TEXT

#include <string>
#include <vector>
#include "../../ska/Graphic/Rectangle.h"
#include "DynamicWindowArea.h"
#include "../../ska/Graphic/Texture.h"

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
    vector<ska::Texture> m_stext;
    ska::Texture m_topArrow, m_botArrow, m_cursor, m_scrollBar;
	ska::Rectangle m_posTopArrow, m_posBotArrow, m_posCursor;
    SDL_Color m_color;
};

#endif
