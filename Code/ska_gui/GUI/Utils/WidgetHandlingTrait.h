#pragma once
#include "WidgetContainer.h"
#include "Utils/VectorUtils.h"

namespace ska {
	template <class SubWidget, bool = std::is_base_of<IHandledWidget, SubWidget>::value>
	struct WidgetHandlingTrait {
		static void manageHandledAdd(std::unique_ptr<SubWidget>&& w, WidgetContainer<std::unique_ptr<Widget>>& handledWidgets, std::vector<std::unique_ptr<Widget>>& widgets, WidgetContainer<Widget*>& globalList) {
			auto widget = static_cast<IHandledWidget*>(w.get());
			auto activeWidget = !widget->isMaskEmpty();
			if (activeWidget) {
				ska::Widget* rawW = w.get();
				handledWidgets.push_back(std::move(w));
				globalList.push_back(std::move(rawW));
			} else {
				WidgetHandlingTrait<SubWidget, false>::manageHandledAdd(std::move(w), handledWidgets, widgets, globalList);
			}
		}

        static bool manageHandledRemove(SubWidget* w, WidgetContainer<std::unique_ptr<Widget>>& handledWidgets, std::vector<std::unique_ptr<Widget>>& widgets, WidgetContainer<Widget*>& globalList) {
			auto widget = static_cast<IHandledWidget*>(w);
			auto activeWidget = !widget->isMaskEmpty();
			if (activeWidget) {
				auto itToRemove = std::remove_if(handledWidgets.begin(), handledWidgets.end(),
					[w](std::unique_ptr<Widget>& it) {
					return it.get() == w;
				});
				auto itRemoved = itToRemove != handledWidgets.end() ? handledWidgets.erase(itToRemove) : handledWidgets.end();

				auto removed = itRemoved != handledWidgets.end();
				 removed |= ska::VectorUtils::removeValue<WidgetContainer<Widget*>, Widget*>(globalList, w);
				 return removed;
			}

	        return WidgetHandlingTrait<SubWidget, false>::manageHandledRemove(w, handledWidgets, widgets, globalList);
        }
	};

	template <class SubWidget>
	struct WidgetHandlingTrait<SubWidget, false> {
		static void manageHandledAdd(std::unique_ptr<SubWidget>&& w, WidgetContainer<std::unique_ptr<Widget>>&, std::vector<std::unique_ptr<Widget>>& widgets, WidgetContainer<Widget*>& globalList) {
			ska::Widget* rawW = w.get();
			widgets.push_back(std::move(w));
			globalList.push_back(std::move(rawW));
		}

        static bool manageHandledRemove(SubWidget* w, WidgetContainer<std::unique_ptr<Widget>>&, std::vector<std::unique_ptr<Widget>>& widgets, WidgetContainer<Widget*>& globalList) {
			auto itRemoved = widgets.erase(std::remove_if(widgets.begin(), widgets.end(),
                             [w](std::unique_ptr<Widget>& it){
                                return it.get() == w;
			}),
				widgets.end());
			auto removed = itRemoved != widgets.end();
			removed |= ska::VectorUtils::removeValue(globalList, w);
			return removed;
		}
	};
}
