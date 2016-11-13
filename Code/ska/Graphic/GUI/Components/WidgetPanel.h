#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <functional>
#include <bitset>
#include <tuple>

#include "HandledWidget.h"
#include "../../../Utils/TupleUtils.h"

namespace ska {
	template <class ... HL>
	class WidgetPanel : public HandledWidget<HL...> {
	public:
		WidgetPanel() = default;

		WidgetPanel(Widget& parent) : HandledWidget<HL...>(parent) {
		}

		WidgetPanel(Widget& parent, Point<int>& position) : HandledWidget<HL...>(parent, position) {
		}

		template <class SubWidget>
		void addWidget(std::unique_ptr<SubWidget>& w) {
			bool empty = true;
			if (std::is_base_of<IHandledWidget, SubWidget>::value && !(empty = ((IHandledWidget&)(*w.get())).isMaskEmpty())) {
				m_handledWidgets.push_back(std::move(w));
			} else {
				m_widgets.push_back(std::move(w));
			}
		}

		void clear() {
			m_widgets.clear();
			m_handledWidgets.clear();
		}

		/* Called from GUI */
		virtual bool notify(IWidgetEvent& e) override {
			if(!isVisible()) {
				return false;
			}
			bool result = false;
			for (auto& w : m_handledWidgets) {
				result |= w->notify(e);
				if (e.stopped() == StopType::STOP_WIDGET) {
					return result;
	 			}
			}
			result |= directNotify(e);
			if (result) {
				/* Handled by Widget */
				e.stopPropagation(StopType::STOP_WIDGET);
			}
			return result;
		}

		virtual bool directNotify(IWidgetEvent& e) override {
			return HandledWidget<HL...>::notify(e);
		}

		void display() const {
			for (const auto& w : m_widgets) {
				w->display();
			}

			for (const auto& w : m_handledWidgets) {
				w->display();
			}
		}

		virtual ~WidgetPanel() = default;

		void switchTextureAndMemorize() {}
		void resetTexture() {}

	private:
		std::vector<std::unique_ptr<Widget>> m_widgets;
		std::vector<std::unique_ptr<Widget>> m_handledWidgets;

	};
}
