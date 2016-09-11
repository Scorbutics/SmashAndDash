#pragma once

#include <vector>
#include <memory>
#include "Widget.h"

namespace ska {
	class HasWidgets {
	public:
		HasWidgets() = default;
		void addWidget(std::unique_ptr<Widget>&& w);
		void clear();
		virtual ~HasWidgets() = default;

	protected:
		std::vector<std::unique_ptr<Widget>> m_widgets;

	};
}