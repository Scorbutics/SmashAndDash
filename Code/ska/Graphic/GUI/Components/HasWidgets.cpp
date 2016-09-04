#include "HasWidgets.h"
#include "../../../Utils/RectangleUtils.h"

void ska::HasWidgets::addWidget(std::unique_ptr<ska::Widget>&& w) {
	m_widgets.push_back(std::move(w));
}

void ska::HasWidgets::clear() {
	m_widgets.clear();
}

ska::Widget* ska::HasWidgets::getElementAtPos(const ska::Point<int>& relativePos) const {
	for (const auto& w : m_widgets) {
		const auto& box = w->getBox();
		if (ska::RectangleUtils::isPositionInBox(relativePos, box)) {
			return w.get();
		}
	}
	return nullptr;
}
