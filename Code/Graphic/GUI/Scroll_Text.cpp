#include <iostream>
#include <string>


#include "Scroll_Text.h"
#include "../../Utils\IDs.h"
#include "DialogMenu.h"
#include "../../ska/Inputs/InputContextManager.h"
#include "../../Utils\ChargementImages.h"
#include "../../ska/Utils/RectangleUtils.h"

Scroll_Text::Scroll_Text(const ska::InputContextManager& icm, DialogMenu &parent, const std::string& buttonAspect, int height, int width, const std::vector<std::string>& text, int fontSize, ska::Rectangle relativePos) : 
DynamicWindowArea(parent),
m_playerICM(icm) {

	m_topArrow.load(buttonAspect + "toparrow.png", DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, 128);
	m_botArrow.load(buttonAspect + "botarrow.png", DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, 128);
	m_scrollBar.load(buttonAspect + "scrollbar.png", DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, 128); //L'idéal est que cette image fasse "fontSize" de hauteur.
	m_cursor.load(buttonAspect + "cursor.png", DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE, 128);

    m_color.r = 0;
    m_color.g = 0;
    m_color.b = 0;
    m_stext.resize(text.size());
	for (unsigned int i = 0; i < m_stext.size(); i++) {
		m_stext[i].loadFromText(m_fontSize, text[i], m_color);
	}

    m_linesNumber = height/fontSize;
    m_width = width;
    m_fontSize = fontSize;
    m_start = 0;
    m_type = BUTTON_SCROLL_TEXT;
    m_text = text;
    m_active = parent.isVisible();

    m_relativePos.x = relativePos.x;
    m_relativePos.y = relativePos.y;
    
	m_posTopArrow.x = m_width + (m_parent.getRect()).x + m_relativePos.x;
	m_posTopArrow.y = (m_parent.getRect()).y + m_relativePos.y;
    m_posTopArrow.w = m_topArrow.getWidth();
    m_posTopArrow.h = m_topArrow.getHeight();

    m_posBotArrow = m_posTopArrow;
    m_posBotArrow.y += m_linesNumber * m_fontSize - m_botArrow.getHeight();
    m_posBotArrow.w = m_botArrow.getWidth();
    m_posBotArrow.h = m_botArrow.getHeight();

    m_posCursor = m_posTopArrow;
    m_posCursor.y += m_topArrow.getHeight();
    m_posCursor.w = m_cursor.getWidth();
    m_posCursor.h = m_cursor.getHeight();

}

void Scroll_Text::display() const
{
	if (!m_parent.isVisible()) {
		return;
	}
    

	ska::Point<int> buf = m_relativePos;
	ska::Rectangle bufScrollBar;
	buf.x += (m_parent.getRect()).x;
	buf.y += (m_parent.getRect()).y;

    bufScrollBar.x = buf.x;
	bufScrollBar.y = buf.y;
    bufScrollBar.x += m_width;
    bufScrollBar.h = m_posBotArrow.y - m_posTopArrow.y;
    bufScrollBar.w = m_scrollBar.getWidth();


    for(unsigned int i = m_start; i < m_start + m_linesNumber; i++) {
		if (i < m_stext.size()) {
			m_stext[i].render(buf.x, buf.y);
		}
            
		m_scrollBar.render(bufScrollBar.x, bufScrollBar.y);
        buf.y += m_fontSize;
        bufScrollBar.y += m_fontSize;
    }

	m_topArrow.render(m_posTopArrow.x, m_posTopArrow.y);
	m_cursor.render(m_posCursor.x, m_posCursor.y);
	m_botArrow.render(m_posBotArrow.x, m_posBotArrow.y);

}


void Scroll_Text::refresh() {
	const ska::InputActionContainer& in = m_playerICM.getActions();
	const ska::InputRange& mouseClickPos = m_playerICM.getRanges()[ska::InputRangeType::MousePos];
	ska::Point<int> buf = m_relativePos;
	ska::Rectangle bufScrollBar;
	buf.x += (m_parent.getRect()).x;
	buf.y += (m_parent.getRect()).y;

	bufScrollBar.x = buf.x;
	bufScrollBar.y = buf.y;
	bufScrollBar.x += m_width;
	bufScrollBar.h = m_posBotArrow.y - m_posTopArrow.y;
	bufScrollBar.w = m_scrollBar.getWidth();

	m_active = true;

	if (m_parent.isMoving()) {
		m_posTopArrow.x = m_width + (m_parent.getRect()).x + m_relativePos.x;
		m_posTopArrow.y = (m_parent.getRect()).y + m_relativePos.y;
		m_posBotArrow = m_posTopArrow;
		m_posBotArrow.y += m_linesNumber * m_fontSize - m_botArrow.getHeight();
		m_posCursor = m_posTopArrow;
		m_posCursor.y += m_topArrow.getHeight();
	}
	m_posCursor.y = m_posTopArrow.y + m_topArrow.getHeight() + m_start * ((int)m_text.size());

	if (ska::RectangleUtils::isPositionInBox(mouseClickPos, m_posTopArrow))
	{
		if (m_lastMouseState == 1 && !in[ska::InputAction::LClic]) //Si on clique sur la fleche du haut
		{
			m_start--;
			if(m_start < 0)
				m_start = 0;
		}
	}
	else if (ska::RectangleUtils::isPositionInBox(mouseClickPos, m_posBotArrow))
	{
		if (m_lastMouseState == 1 && !in[ska::InputAction::LClic]) //Si on clique sur la fleche du bas
		{
			m_start++;
			if(m_start > (int)m_linesNumber)
				m_start = m_linesNumber;
		}
	}
	else if (ska::RectangleUtils::isPositionInBox(mouseClickPos, bufScrollBar))
	{
		//Si on clique sur la barre du curseur
		if (in[ska::InputAction::LClic]) {
			m_start = ((int)mouseClickPos.y - m_posTopArrow.y) / ((int)m_text.size()) - 1;

			if(m_start > (int)m_linesNumber)
				m_start = m_linesNumber;
			else if(m_start < 0)
				m_start = 0;

		}
	}

	m_posCursor.y = m_posTopArrow.y + m_topArrow.getHeight() + m_start * ((int)m_text.size());

	m_lastMouseState = in[ska::InputAction::LClic];
}

Scroll_Text::~Scroll_Text()
{
}
