#include "HasWidgets.h"
#include "../../../Utils/RectangleUtils.h"

void ska::HasWidgets::addWidget(std::unique_ptr<ska::Widget>&& w) {
	m_widgets.push_back(std::move(w));
}

void ska::HasWidgets::clear() {
	m_widgets.clear();
}

ska::Widget* ska::HasWidgets::getElementAtPos(const ska::Point<int>& pos) const {
	for (const auto& w : m_widgets) {
		const auto& box = w->getBox();
		const auto& absPos = w->getAbsolutePosition();
		if (ska::RectangleUtils::collisionBoxABoxB(ska::Rectangle{ pos.x, pos.y, 1, 1 }, ska::Rectangle{absPos.x, absPos.y, box.w, box.h})) {
			return w.get();
		}
	}
	return nullptr;
}
