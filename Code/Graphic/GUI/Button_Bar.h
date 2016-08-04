#ifndef DEF_BUTTON_BAR
#define DEF_BUTTON_BAR

#include "Button.h"
class WGameCore;

class Button_Bar : public DynamicWindowArea
{
    public:
		Button_Bar(DialogMenu* parent, ska::Rectangle relativePos, std::string styleName, int* variable, std::vector<int> values, std::vector<std::string> displayedText, int fontSize, std::string key);
    virtual void refresh();
	virtual void display();
    virtual std::string getKey(){return "";};
    virtual void forceValue(unsigned int){};
    virtual ~Button_Bar();

    private:
    ska::Texture m_leftArrow, m_rightArrow, m_buttonStyle, m_cursor;
	ska::Point<int> m_leftPos, m_rightPos, m_cursorPos;
    int* m_variable;
    std::vector<int> m_values;
    std::unique_ptr<Button> m_button;
    unsigned int m_index;

};

#endif
