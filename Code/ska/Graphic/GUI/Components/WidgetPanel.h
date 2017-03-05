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

		template <class SubWidget, typename std::enable_if<std::is_base_of<IHandledWidget, SubWidget>::value>::type* dummy = 0>
		SubWidget* addWidget(std::unique_ptr<SubWidget>& w) {
			w->setPriority(static_cast<int>(m_globalList.size()));
			if (!reinterpret_cast<IHandledWidget&>(*w.get()).isMaskEmpty()) {
				m_handledWidgets.push_back(std::move(w));
				m_globalList.push_back(m_handledWidgets.back().get());
			} else {
				m_widgets.push_back(std::move(w));
				m_globalList.push_back(m_widgets.back().get());
			}
			return reinterpret_cast<SubWidget*>(m_globalList.back());
		}

		template <class SubWidget, typename std::enable_if<!std::is_base_of<IHandledWidget, SubWidget>::value>::type* dummy = 0>
		SubWidget* addWidget(std::unique_ptr<SubWidget>& w) {
			w->setPriority(static_cast<int>(m_globalList.size()));
			m_widgets.push_back(std::move(w));
			m_globalList.push_back(m_widgets.back().get());
			
			return reinterpret_cast<SubWidget*>(m_globalList.back());
		}

		/* Called from GUI */
		virtual bool notify(IWidgetEvent& e) override {
			/* If the current WidgetPanel doesn't accept the event, neither of his children do. */
			if (!HandledWidget<HL...>::accept(e)) {
				return false;
			}

			bool result = false;
			bool stopped = false;
			for (auto& w : m_handledWidgets) {
				const auto nextNotify = w->notify(e);
				result |= nextNotify;
				if (e.stopped() == STOP_WIDGET) {
					stopped = true;
					break;
	 			}
			}
			
			if (stopped) {
				e.stopPropagation(NOT_STOPPED);
			}
			
			result |= directNotify(e);

			if (result || stopped) {
				/* Handled by Widget */
				e.stopPropagation(STOP_WIDGET);
			}
			return result;
		}

		void showWidgets(bool b) {
			for(auto w : m_globalList) {
				w->show(b);
			}
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

		void clear() {
			m_widgets.clear();
			m_handledWidgets.clear();
			m_globalList.clear();
		}

	protected:
		Widget* getWidget(size_t index) {
			return m_globalList[index];
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
				sort(m_globalList.begin(), m_globalList.end(), Drawable::staticOperatorInf);
				std::sort(m_handledWidgets.begin(), m_handledWidgets.end(), comparatorDesc);
			} else {
				sort(m_globalList.begin(), m_globalList.end(), Drawable::staticOperatorSup);
				std::sort(m_handledWidgets.begin(), m_handledWidgets.end(), comparatorAsc);
			}
		}

		std::vector<std::unique_ptr<Widget>> m_widgets;
		std::vector<std::unique_ptr<Widget>> m_handledWidgets;
		std::vector<Widget*> m_globalList;

	};
}
