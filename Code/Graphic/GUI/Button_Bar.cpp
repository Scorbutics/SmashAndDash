#include "Button_Bar.h"
#include "../../Utils\ChargementImages.h"
#include "../../Gameplay\WGameCore.h"
#include "../../ska/Utils/RectangleUtils.h"
#include "DialogMenu.h"

Button_Bar::Button_Bar(DialogMenu* parent, ska::Rectangle relativePos, std::string styleName, int* variable, std::vector<int> values, std::vector<std::string> displayedText, int fontSize, std::string key) : DynamicWindowArea(parent)
{
	m_buttonStyle.load(styleName, DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);
	m_leftArrow.load("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"scroll_leftarrow.png", DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);
	m_rightArrow.load("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"scroll_rightarrow.png", DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);
	m_cursor.load("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"scroll_cursor.png", DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);

	ska::Rectangle buf = relativePos;
    buf.x -= m_cursor.getWidth()/2;
    buf.y += m_buttonStyle.getHeight();
    m_button = std::unique_ptr<Button>(new Button(parent, buf, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"button.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonpressed.png", variable, values, displayedText, fontSize, key, true));

    m_relativePos = relativePos;
    m_leftPos.x = m_relativePos.x - m_leftArrow.getWidth();
    m_leftPos.y = m_relativePos.y;
    m_rightPos.x = m_relativePos.x + m_buttonStyle.getWidth();
    m_rightPos.y = m_relativePos.y;
    m_cursorPos = m_relativePos;

    m_variable = variable;
    m_values = values;
    m_button->forceIndexFromCurrentValue();
}

void Button_Bar::display()
{

    if(!m_parent->isVisible())
        return;
    else
        m_active = true;

    

//Coordonnées relatives -> absolues
	ska::Point<int> buf = m_relativePos, absoluteCursorPos = m_cursorPos, absoluteLeftPos = m_leftPos, absoluteRightPos = m_rightPos;
	buf.x += (m_parent->getRect()).x;
	buf.y += (m_parent->getRect()).y;
	absoluteLeftPos.x += (m_parent->getRect()).x;
	absoluteLeftPos.y += (m_parent->getRect()).y;
	absoluteRightPos.x += (m_parent->getRect()).x + m_buttonStyle.getWidth();
	absoluteRightPos.y += (m_parent->getRect()).y;
	absoluteCursorPos.x += (m_parent->getRect()).x;
	absoluteCursorPos.y += (m_parent->getRect()).y;


	m_buttonStyle.render(buf.x, buf.y);
    buf.x += m_buttonStyle.getWidth();
	m_buttonStyle.render(buf.x, buf.y);
	m_leftArrow.render(absoluteLeftPos.x, absoluteLeftPos.y);
	m_rightArrow.render(absoluteRightPos.x, absoluteRightPos.y);
	m_cursor.render(absoluteCursorPos.x, absoluteCursorPos.y);
    m_button->display();
}

void Button_Bar::refresh()
{
	WGameCore& wScreen = WGameCore::getInstance();
	const ska::InputActionContainer& in = wScreen.getActions();
	const ska::InputRange& mousePos = wScreen.getRanges()[ska::InputRangeType::MousePos];

	//Coordonnées relatives -> absolues
	ska::Point<int> buf = m_relativePos, absoluteCursorPos = m_cursorPos;
	buf.x += (m_parent->getRect()).x;
	buf.y += (m_parent->getRect()).y;

	ska::Rectangle absoluteLeftPos;
	absoluteLeftPos.x = m_leftPos.x;
	absoluteLeftPos.y = m_leftPos.y;

	ska::Rectangle absoluteRightPos;
	absoluteRightPos.x = m_rightPos.x;
	absoluteRightPos.y = m_rightPos.y;


	absoluteLeftPos.x += (m_parent->getRect()).x;
	absoluteLeftPos.y += (m_parent->getRect()).y;
	absoluteRightPos.x += (m_parent->getRect()).x + m_buttonStyle.getWidth();
	absoluteRightPos.y += (m_parent->getRect()).y;
	absoluteCursorPos.x += (m_parent->getRect()).x;
	absoluteCursorPos.y += (m_parent->getRect()).y;


	//Si on clique sur la flèche gauche
	if (ska::RectangleUtils::isPositionInBox(mousePos, absoluteLeftPos) && in[ska::InputAction::LClic])
	{
		m_index--;
		m_cursorPos.x = (int)(m_relativePos.x + m_index%(m_values.size()) * (((2*m_buttonStyle.getWidth())-m_cursor.getWidth()) / (float) m_values.size()+1));
		*m_variable = m_values[m_index%(m_values.size())];
	}
	//Sinon si on clique sur la flèche droite
	else if (ska::RectangleUtils::isPositionInBox(mousePos, absoluteRightPos) && in[ska::InputAction::LClic])
	{
		m_index++;
		m_cursorPos.x = (int)(m_relativePos.x + m_index%(m_values.size()) * (((2 * m_buttonStyle.getWidth())-m_cursor.getWidth()) / (float) m_values.size()+1));
		*m_variable = m_values[m_index%(m_values.size())];
	}

	m_button->refresh();
}

Button_Bar::~Button_Bar()
{
}
