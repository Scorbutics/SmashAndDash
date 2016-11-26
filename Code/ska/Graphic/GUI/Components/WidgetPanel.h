#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <deque>
#include <functional>
#include <algorithm>
#include <bitset>
#include <tuple>

#include "HandledWidget.h"
#include "../../../Utils/TupleUtils.h"

namespace ska {
	template <class ... HL>
	class WidgetPanel : public HandledWidget<HL...> {
	public:
		WidgetPanel() = default;

		WidgetPanel(Widget& parent) : 
			HandledWidget<HL...>(parent) {
		}

		WidgetPanel(Widget& parent, Point<int>& position) : 
			HandledWidget<HL...>(parent, position) {
		}

		template <class SubWidget>
		void addWidget(std::unique_ptr<SubWidget>& w) {
			bool empty = true;
			w->setPriority((int)(m_globalList.size()));
			if (std::is_base_of<IHandledWidget, SubWidget>::value && !(empty = ((IHandledWidget&)(*w.get())).isMaskEmpty())) {
				m_handledWidgets.push_back(std::move(w));
				m_globalList.push_back(m_handledWidgets.back().get());
			} else {
				m_widgets.push_back(std::move(w));
				m_globalList.push_back(m_widgets.back().get());
			}
		}

		void clear() {
			m_widgets.clear();
			m_handledWidgets.clear();
			m_globalList.clear();
		}

		/* Called from GUI */
		virtual bool notify(IWidgetEvent& e) override {
			if(!Widget::isVisible()) {
				return false;
			}
			bool result = false;
			for (auto& w : m_handledWidgets) {
				const auto nextNotify = w->notify(e);
				result |= nextNotify;
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

		void display() const override {
			for (auto w = m_globalList.begin(); w != m_globalList.end(); w++) {
				(*w)->display();
			}
		}

		Widget* backWidget() {
			return m_globalList.back();
		}

		virtual ~WidgetPanel() = default;

		void switchTextureAndMemorize() {}
		void resetTexture() {}

		void resort() {
			this->sortZIndexWidgets(false);	
		}

	private:
		void sortZIndexWidgets(bool asc) {
			auto comparatorAsc = [](const std::unique_ptr<Widget>& w1, const std::unique_ptr<Widget>& w2) {
				return (w1->getPriority() < w2->getPriority());
			};
			auto comparatorDesc = [](const std::unique_ptr<Widget>& w1, const std::unique_ptr<Widget>& w2) {
				return (w1->getPriority() > w2->getPriority());
			};
			if (asc) {
				std::sort(m_globalList.begin(), m_globalList.end(), ska::Drawable::staticOperatorInf);
				std::sort(m_handledWidgets.begin(), m_handledWidgets.end(), comparatorDesc);
			} else {
				std::sort(m_globalList.begin(), m_globalList.end(), ska::Drawable::staticOperatorSup);
				std::sort(m_handledWidgets.begin(), m_handledWidgets.end(), comparatorAsc);
			}
		}

		std::deque<std::unique_ptr<Widget>> m_widgets;
		std::deque<std::unique_ptr<Widget>> m_handledWidgets;
		std::deque<Widget*> m_globalList;

	};
}
