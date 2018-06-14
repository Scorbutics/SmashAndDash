#include "DialogMenu.h"
#include "GUI/GUI.h"
#include "GUI/Components/Concrete/Button.h"
#include "GUI/Components/Concrete/ScrollingMultiLineLabel.h"
#include "../../Utils/IDs.h"
#include "GUI/GUI.h"

DialogMenu::DialogMenu(ska::Widget& parent, const std::string& text, const std::string& name, const ska::Rectangle rect, const unsigned int fontSize, const bool scroll, const int timeout) :
TimeScrollableWindowIG<ska::KeyEventListener>(parent, rect),
m_fontSize(fontSize),
m_timeout(timeout),
m_name(name),
m_scroll(scroll) {
	addWidget<ska::ScrollingMultiLineLabel>(text, m_fontSize, 1.F, ska::Point<int>(16, 16));
}

void DialogMenu::render(const ska::Renderer& renderer) const{
	TimeScrollableWindowIG<ska::KeyEventListener>::render(renderer);
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
