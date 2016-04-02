#include "Button.h"
#include "../../Gameplay\WGameCore.h"
#include "../../Utils\ChargementImages.h"
#include "../../ska/Utils/RectangleUtils.h"
#include "DialogMenu.h"

using namespace std;

Button::Button(DialogMenu* parent, ska::Rectangle relativePos, std::string styleName, std::string styleNamePressed, int* variable, std::vector<int> values, std::vector<std::string> displayedText, int fontSize, std::string key, bool bstatic) : DynamicWindowArea(parent)
{
    m_static = bstatic;
    m_index = 0;
    m_buttonStyle.load(styleName, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);
    m_buttonStylePressed.load(styleNamePressed, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);
    m_fontSize = fontSize;
    m_displayedText = displayedText;
    m_relativePos = relativePos;
    m_relativePos.w = m_buttonStyle.getWidth();
    m_relativePos.h = m_buttonStyle.getHeight();
    m_couleur.r = m_couleur.g = m_couleur.b = 0;
    m_variable = variable;
    m_values = values;
    m_key = key;
    m_text = m_displayedText[m_index%(m_values.size())];
	m_stext.loadFromText(m_fontSize, m_text, m_couleur);
}

void Button::setVariable(int* variable)
{
    m_variable = variable;
}

void Button::forceValue(unsigned int index)
{
    if(index < m_values.size())
		*m_variable = m_values[index];
    m_text = m_displayedText[m_index%(m_values.size())];

    m_stext.loadFromText(m_fontSize, m_text, m_couleur);
}


void Button::display()
{

    if(!m_parent->isVisible())
        return;
    else
        m_active = true;

	WGameCore& wScreen = WGameCore::getInstance();
	const ska::InputRange& mousePos = wScreen.getRanges()[ska::InputRangeType::MousePos];
	const ska::InputActionContainer& in = wScreen.getActions();

	ska::Rectangle buf = m_relativePos, textPos = m_relativePos;
	buf.x += (m_parent->getRect()).x;
	buf.y += (m_parent->getRect()).y;
    textPos.w = m_stext.getWidth();
    textPos.h = m_stext.getHeight();

	textPos = ska::RectangleUtils::posToCenterPicture(textPos, buf);

	if (!m_static && ska::RectangleUtils::isPositionInBox(mousePos, buf) && in[ska::InputAction::LClic]) //Si on presse le bouton
    {
		m_buttonStylePressed.render(buf.x, buf.y);
        textPos.x += 2; //pour faire un effet "bouton enfoncé", on décale le texte vers le bas droite
        textPos.y += 2;
    }
    else
        m_buttonStyle.render(buf.x, buf.y);

	m_stext.render(textPos.x, textPos.y);
}

void Button::refresh()
{
	WGameCore& wScreen = WGameCore::getInstance();
	const ska::InputRange& mousePos = wScreen.getRanges()[ska::InputRangeType::MousePos];
	const ska::InputActionContainer& in = wScreen.getActions();
	ska::Rectangle buf = m_relativePos, textPos = m_relativePos;
	buf.x += (m_parent->getRect()).x;
	buf.y += (m_parent->getRect()).y;
	textPos.w = m_stext.getWidth();
	textPos.h = m_stext.getHeight();

	textPos = ska::RectangleUtils::posToCenterPicture(textPos, buf);

	if (!m_static && ska::RectangleUtils::isPositionInBox(mousePos, buf) && in[ska::InputAction::LClic] && m_variable != NULL) //Si on a cliqué sur le bouton (différent de l'état "bouton pressé" !)
	{
		//m_parent->pause();
		m_index++;
		*m_variable = m_values[m_index%(m_values.size())];
		m_text = m_displayedText[m_index%(m_values.size())];
		m_stext.loadFromText(m_fontSize, m_text, m_couleur);
	}

	if (m_variable != NULL && *m_variable != m_values[m_index % (m_values.size())])
	{
		this->forceIndexFromCurrentValue();
		m_text =  m_displayedText[m_index%(m_values.size())];
		m_stext.loadFromText(m_fontSize, m_text, m_couleur);
	}
}

std::string Button::getKey()
{
    return m_key;
}

void Button::forceIndexFromCurrentValue()
{
    for(unsigned int i = 0; i < m_values.size(); i++)
        if(m_values[i] == *m_variable)
            m_index = i;
}

Button::~Button()
{
}
