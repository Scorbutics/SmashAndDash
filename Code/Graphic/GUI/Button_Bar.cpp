#include "Button_Bar.h"
#include "../../Utils\ChargementImages.h"
#include "../../Gameplay\WGameCore.h"

using namespace std;

Button_Bar::Button_Bar(DialogMenu* parent, SDL_Rect relativePos, std::string styleName, int* variable, std::vector<int> values, std::vector<std::string> displayedText, int fontSize, std::string key) : DynamicWindowArea(parent)
{
    m_buttonStyle.load(styleName, T_RED, T_GREEN, T_BLUE);
    m_leftArrow.load("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"scroll_leftarrow.png", T_RED, T_GREEN, T_BLUE);
    m_rightArrow.load("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"scroll_rightarrow.png", T_RED, T_GREEN, T_BLUE);
    m_cursor.load("."FILE_SEPARATOR"Menu"FILE_SEPARATOR"scroll_cursor.png", T_RED, T_GREEN, T_BLUE);

    SDL_Rect buf = relativePos;
    buf.x -= m_cursor.getWidth()/2;
    buf.y += m_buttonStyle.getHeight();
    m_button = unique_ptr<Button>(new Button(parent, buf, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"button.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonpressed.png", variable, values, displayedText, fontSize, key, true));

    m_relativePos = relativePos;
    m_relativePos.w = 2*m_buttonStyle.getWidth();
    m_relativePos.h = m_buttonStyle.getHeight();
    m_leftPos.x = m_relativePos.x - m_leftArrow.getWidth();
    m_leftPos.y = m_relativePos.y;
    m_leftPos.w = m_leftArrow.getWidth();
    m_leftPos.h = m_leftArrow.getHeight();
    m_rightPos.x = m_relativePos.x + m_buttonStyle.getWidth();
    m_rightPos.y = m_relativePos.y;
    m_rightPos.w = m_rightArrow.getWidth();
    m_rightPos.h = m_rightArrow.getHeight();
    m_cursorPos = m_relativePos;
    m_cursorPos.w = m_cursor.getWidth();
    m_cursorPos.h = m_cursor.getHeight();

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
    SDL_Rect buf = m_relativePos, absoluteCursorPos = m_cursorPos, absoluteLeftPos = m_leftPos, absoluteRightPos = m_rightPos;
    buf.x += (m_parent->getPos())->x;
    buf.y += (m_parent->getPos())->y;
    absoluteLeftPos.x += (m_parent->getPos())->x;
    absoluteLeftPos.y += (m_parent->getPos())->y;
    absoluteRightPos.x += (m_parent->getPos())->x + m_buttonStyle.getWidth();
    absoluteRightPos.y += (m_parent->getPos())->y;
    absoluteCursorPos.x += (m_parent->getPos())->x;
    absoluteCursorPos.y += (m_parent->getPos())->y;


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
	MouseInput* in = wScreen.getInputListener().getMouseInput();
	//Coordonnées relatives -> absolues
	SDL_Rect buf = m_relativePos, absoluteCursorPos = m_cursorPos, absoluteLeftPos = m_leftPos, absoluteRightPos = m_rightPos, mousePos = in->getMousePos();
	buf.x += (m_parent->getPos())->x;
	buf.y += (m_parent->getPos())->y;
	absoluteLeftPos.x += (m_parent->getPos())->x;
	absoluteLeftPos.y += (m_parent->getPos())->y;
	absoluteRightPos.x += (m_parent->getPos())->x + m_buttonStyle.getWidth();
	absoluteRightPos.y += (m_parent->getPos())->y;
	absoluteCursorPos.x += (m_parent->getPos())->x;
	absoluteCursorPos.y += (m_parent->getPos())->y;


	//Si on clique sur la flèche gauche
	if(IsPositionInBox(&mousePos, &absoluteLeftPos) && in->mouseClick(SDL_BUTTON_LEFT))
	{
		m_index--;
		m_cursorPos.x = (int)(m_relativePos.x + m_index%(m_values.size()) * (((2*m_buttonStyle.getWidth())-m_cursor.getWidth()) / (float) m_values.size()+1));
		*m_variable = m_values[m_index%(m_values.size())];
	}
	//Sinon si on clique sur la flèche droite
	else if(IsPositionInBox(&mousePos, &absoluteRightPos) && in->mouseClick(SDL_BUTTON_LEFT))
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
