#ifndef DEF_BUTTON
#define DEF_BUTTON

#include <sstream>
#include "ChargementImages.h"
#include "DynamicWindowArea.h"
#include <SDL2/SDL_ttf.h>

class WGameCore;

class Button : public DynamicWindowArea
{
    public:
    Button(DialogMenu* parent, SDL_Rect relativePos, std::string styleName, std::string styleNamePressed, int* variable, std::vector<int> values, std::vector<std::string> displayedText, int fontSize, std::string key, bool bstatic);
    void setVariable(int* variable);
    virtual void forceValue(unsigned int index);
    virtual void refresh();
	virtual void display();
    virtual std::string getKey();
    void forceIndexFromCurrentValue();
    virtual ~Button();


    private:
    bool m_static;
    std::string m_text, m_key;
    int m_fontSize;
    unsigned int m_index;
    Texture m_stext, m_buttonStyle, m_buttonStylePressed;
    SDL_Rect m_rectButton;
    int *m_variable;
    std::vector<int> m_values;
    std::vector<std::string> m_displayedText;
    SDL_Color m_couleur;
    TTF_Font *m_font;

};
#endif
