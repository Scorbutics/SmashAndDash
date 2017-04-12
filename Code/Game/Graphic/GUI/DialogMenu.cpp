#include "DialogMenu.h"
#include "GUI/Components/Concrete/Button.h"
#include "GUI/Components/Concrete/ScrollingMultiLineLabel.h"
#include "../../Utils/IDs.h"

DialogMenu::DialogMenu(ska::TimeObservable& timeObs, ska::MouseObservable& mouseObs, ska::KeyObservable& keyObs, const std::string& text, const std::string& name, const ska::Rectangle rect, const unsigned int fontSize, const bool scroll, const int timeout) :
TimeScrollableWindowIG<ska::KeyEventListener>(&timeObs, &mouseObs, &keyObs, rect, ska::Button::MENU_DEFAULT_THEME_PATH + "menu"),
m_fontSize(fontSize),
m_timeout(timeout),
m_name(name),
m_scroll(scroll) {
	addWidget(std::make_unique<ska::ScrollingMultiLineLabel>(*this, text, m_fontSize, 1.F, ska::Point<int>(16, 16)));
}

int DialogMenu::getPriority() const {
	return GUI_DEFAULT_DISPLAY_PRIORITY;
}

void DialogMenu::display() const {
    TimeScrollableWindowIG<ska::KeyEventListener>::display();
}

bool DialogMenu::isVisible() const {
    return TimeScrollableWindowIG<ska::KeyEventListener>::isVisible();
}


const std::string& DialogMenu::getName() const {
    return m_name;
}
