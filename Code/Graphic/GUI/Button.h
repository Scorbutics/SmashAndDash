#pragma once

#include <sstream>
#include <vector>
#include "../../ska/Graphic/Texture.h"
#include "DynamicWindowArea.h"

namespace ska {
	class InputContextManager;
}

//TODO : renommer en TextButton + hériter de SpriteButton
class Button : public DynamicWindowArea
{
public:
	Button(const ska::InputContextManager& icm, DialogMenu& parent, ska::Rectangle relativePos, std::string styleName, std::string styleNamePressed, int* variable, std::vector<int> values, std::vector<std::string> displayedText, int fontSize, std::string key, bool bstatic);
    void setValue(unsigned int index);
    virtual void refresh() override;
	virtual void display() const override;
    virtual std::string getKey();
    void forceIndexFromCurrentValue();
    virtual ~Button();


    private:
    bool m_static;
    std::string m_text, m_key;
    int m_fontSize;
    unsigned int m_index;
    ska::Texture m_stext, m_buttonStyle, m_buttonStylePressed;
	ska::Rectangle m_rectButton;
    int *m_variable;
    std::vector<int> m_values;
    std::vector<std::string> m_displayedText;
    SDL_Color m_couleur;

	const ska::InputContextManager& m_playerICM;

};

