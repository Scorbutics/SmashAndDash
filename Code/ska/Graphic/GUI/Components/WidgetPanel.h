#pragma once
#include <memory>
#include <vector>
#include <type_traits>
#include <algorithm>


#include "HandledWidget.h"
#include "SortedWidgetVector.h"

namespace ska {

	template <class T>
	using WidgetContainer = std::vector<T>;

	template <class SubWidget, bool = std::is_base_of<IHandledWidget, SubWidget>::value>
	struct WidgetHandlingTrait {
		static void manageHandled(std::unique_ptr<SubWidget>&& w, WidgetContainer<std::unique_ptr<Widget>>& handledWidgets, std::vector<std::unique_ptr<Widget>>& widgets, WidgetContainer<Widget*>& globalList) {
			auto widget = static_cast<IHandledWidget*>(w.get());
			auto activeWidget = !widget->isMaskEmpty();
			if (activeWidget) {
				ska::Widget* rawW = w.get();
				handledWidgets.push_back(std::move(w));
				globalList.push_back(std::move(rawW));
			} else {
				WidgetHandlingTrait<SubWidget, false>::manageHandled(std::move(w), handledWidgets, widgets, globalList);
			}
		}
	};

	template <class SubWidget>
	struct WidgetHandlingTrait<SubWidget, false> {
		static void manageHandled(std::unique_ptr<SubWidget>&& w, WidgetContainer<std::unique_ptr<Widget>>&, std::vector<std::unique_ptr<Widget>>& widgets, WidgetContainer<Widget*>& globalList) {
			ska::Widget* rawW = w.get();
			widgets.push_back(std::move(w));
			globalList.push_back(std::move(rawW));
		}
	};

	template <class ... HL>
	class WidgetPanel : public HandledWidget<HL...> {
	public:
		WidgetPanel() = default;

		explicit WidgetPanel(Widget& parent) : 
			HandledWidget<HL...>(parent) {
		}

		WidgetPanel(Widget& parent, Point<int>& position) : 
			HandledWidget<HL...>(parent, position) {
		}

		template <class SubWidget>
		SubWidget* addWidget(std::unique_ptr<SubWidget>&& w) {
			w->setPriority(static_cast<int>(m_globalList.size()));
			auto result = static_cast<SubWidget*>(w.get());
			WidgetHandlingTrait<SubWidget>::manageHandled(std::move(w), m_handledWidgets, m_widgets, m_globalList);
			m_addedSortedWidgets.push_back(result);

			return result;
		}

		/* Called from GUI */
		virtual bool notify(IWidgetEvent& e) override {		
			/* If the current WidgetPanel doesn't accept the event, neither of his children do. */
			if (!HandledWidget<HL...>::accept(e)) {
				return false;
			}

			auto result = false;
			auto stopped = false;
			std::size_t cursor = 0;
			for (auto& w : m_handledWidgets) {
				/*
				if(m_handledWidgets.organize(w, cursor)) {
					return result;
				}
				*/

				const auto nextNotify = w->notify(e);
				result |= nextNotify;
				if (e.stopped() == STOP_WIDGET) {
					stopped = true;
					break;
	 			}
				cursor++;
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
			for (auto w = m_globalList.cbegin(); w != m_globalList.cend(); ++w) {
				(*w)->display();
			}
		}

		Widget* backAddedWidget() {
			return m_addedSortedWidgets.back();
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
			m_addedSortedWidgets.clear();
		}

	protected:
		Widget* getWidget(size_t index) {
			return m_addedSortedWidgets[index];
		}

	private:
		void sortZIndexWidgets(bool asc) {
			auto comparatorAsc = [](const std::unique_ptr<Widget>& w1, const std::unique_ptr<Widget>& w2) {
				/*auto v1 = w1->isVisible() ? 1 : 0;
				auto v2 = w2->isVisible() ? 1 : 0;

				if (v1 == v2) {*/
					return (w1->getPriority() < w2->getPriority());
				/*}

				return v1 < v2;*/
			};
			auto comparatorDesc = [](const std::unique_ptr<Widget>& w1, const std::unique_ptr<Widget>& w2) {
				/*auto v1 = w1->isVisible() ? 1 : 0;
				auto v2 = w2->isVisible() ? 1 : 0;
				*/
				//if(v1 == v2) {
					return (w1->getPriority() > w2->getPriority());
				//}

				//return v1 < v2;
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
		WidgetContainer<std::unique_ptr<Widget>> m_handledWidgets;
		std::vector<Widget*> m_addedSortedWidgets;
		WidgetContainer<Widget*> m_globalList;

	};
}
