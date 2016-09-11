#include "HasWidgets.h"
#include "../../../Utils/RectangleUtils.h"

void ska::HasWidgets::addWidget(std::unique_ptr<ska::Widget>&& w) {
	m_widgets.push_back(std::move(w));
}

void ska::HasWidgets::clear() {
	m_widgets.clear();
}

