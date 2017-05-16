#include "DialogMenu.h"
#include "GUI/Components/Concrete/Button.h"
#include "GUI/Components/Concrete/ScrollingMultiLineLabel.h"
#include "../../Utils/IDs.h"

DialogMenu::DialogMenu(ska::Widget& parent, const std::string& text, const std::string& name, const ska::Rectangle rect, const unsigned int fontSize, const bool scroll, const int timeout) :
TimeScrollableWindowIG<ska::KeyEventListener>(parent, rect, ska::Button::MENU_DEFAULT_THEME_PATH + "menu"),
m_fontSize(fontSize),
m_timeout(timeout),
m_name(name),
m_scroll(scroll) {
	addWidget<ska::ScrollingMultiLineLabel>(text, m_fontSize, 1.F, ska::Point<int>(16, 16));
}

void DialogMenu::display() const {
    TimeScrollableWindowIG<ska::KeyEventListener>::display();
}

bool DialogMenu::isVisible() const {
    return TimeScrollableWindowIG<ska::KeyEventListener>::isVisible();
}

int DialogMenu::getPriority() const {
	return ska::DrawableFixedPriority::getPriority();
}


const std::string& DialogMenu::getName() const {
    return m_name;
}
