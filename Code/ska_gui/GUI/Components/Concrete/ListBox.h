#pragma once
#include <vector>
#include <sstream>
#include <limits>

#include "Button.h"
#include "Label.h"
#include "../WidgetPanel.h"
#include "../../Events/FocusEvent.h"
#include "../../Windows/DynamicWindowIG.h"

namespace ska {
	class FocusEvent;
	class FocusEventListener;

	template <class T>
	class ListBox : public WidgetPanel<ValueChangedEventListener<T*>, HoverEventListener, ClickEventListener> {
	public:
		ListBox(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, const std::string& buttonStyleName, const Rectangle* clip) : 
			WidgetPanel<ValueChangedEventListener<T*>, HoverEventListener, ClickEventListener>(parent, relativePos),
			m_styleName(buttonStyleName),
			m_selected(nullptr),
			m_fontSize(11) {
				auto button = std::make_unique<Button>(*this, Point<int>(0, 0), placeHolderStyleName, clip, [this] (Widget* tthis, ClickEvent& e) {
					auto w = m_wListBox;
					if(e.getState() != MOUSE_RELEASE) {
						w->focus(true);
						e.setTarget(w);
						return;
					}
					
					auto b = reinterpret_cast<Button&>(*tthis);
					w->move(Rectangle{ 0, b.getBox().h, b.getBox().w, static_cast<int>(m_values.size() + 1)*b.getBox().h });
					w->show(true);
					
				});
				
				ListBox<T>::setPriority(std::numeric_limits<int>::max());

				auto wListbox = std::make_unique<DynamicWindowIG<FocusEventListener>>(*this, Rectangle{ 0, button->getBox().h, button->getBox().w, static_cast<int>(m_values.size() + 1)*button->getBox().h }, "");
				wListbox->show(false);

				wListbox->template addHandler<FocusEventListener>([](Widget* tthis, FocusEvent& e) {
					if (e.getState() == MOUSE_BLUR && (e.getClickedTarget() == nullptr || !tthis->isAParent(*e.getClickedTarget()))) {
						tthis->show(false);
					}
				});

				setWidth(button->getBox().w);
				setHeight(button->getBox().h);

				/*m_gui.addWindow(std::move(m_wListbox));*/
				
				m_wListBox = addWidget(wListbox);
				m_wListBox->setHeight(ListBox<T>::getBox().h);
				m_wListBox->setPriority(1);
				addWidget(button)->setPriority(3);
				m_label = addWidget(std::make_unique<Label>(*this, " ", m_fontSize, Point<int>(5, 1)));
				m_label->setPriority(2);
				
		}

		void clearValue() {
			const auto lastValue = m_selected;
			m_selected = nullptr;
			m_label->modifyText(" ");

			ValueChangedEvent<T*> vce(lastValue, nullptr);
			directNotify(vce);
		}

		void load(std::vector<T>&& values) {
			m_values = std::move(values);
			auto& wListbox = *m_wListBox;
			wListbox.clear();
			static unsigned int  buttonHeight = 12;

			auto b1 = std::make_unique<Button>(wListbox, Point<int>(0, ListBox<T>::getBox().h), m_styleName, nullptr, [&](Widget* tthis, ClickEvent& e) {
				e.stopPropagation(STOP_WIDGET);
				if (e.getState() != MOUSE_RELEASE) {
					return;
				}
				auto button = reinterpret_cast<Button*> (tthis);
				button->getParent()->show(false);
				tthis->focus(true);
				e.setTarget(tthis);
				clearValue();
			});

			wListbox.addWidget(std::make_unique<Label>(wListbox, " ", m_fontSize, Point<int>(10, ListBox<T>::getBox().h)));
			wListbox.addWidget(std::move(b1));
			
			size_t index = 1;
			for (auto& v : m_values) {
				auto b = std::make_unique<Button>(wListbox, Point<int>(0, ListBox<T>::getBox().h + buttonHeight * static_cast<int>(index)), m_styleName, nullptr, [&](Widget* tthis, ClickEvent& e) {
					e.stopPropagation(STOP_WIDGET);
					if (e.getState() != MOUSE_RELEASE) {
						return;
					}
					auto button = reinterpret_cast<Button*> (tthis);
					button->getParent()->show(false);
					tthis->focus(true);
					e.setTarget(tthis);
					forceValue(v);
				});
				

				std::stringstream ss;
				ss << v;
				auto str = ss.str();
				wListbox.addWidget(move(b));
				wListbox.addWidget(std::make_unique<Label>(wListbox, str.empty() ? " " : str, m_fontSize, Point<int>(10, ListBox<T>::getBox().h + buttonHeight * static_cast<int>(index))));

				index++;
			}
			wListbox.setHeight(buttonHeight * (static_cast<int>(m_values.size()) + 1));
		}

		T* getValue() const {
			return m_selected;
		}

		void forceValue(T& v) {
			const auto lastValue = m_selected;
			m_selected = &v;
			std::stringstream ss;
			ss << *m_selected;
			auto str = ss.str();
			m_label->modifyText(str.empty() ? " " : str);
			
			ValueChangedEvent<T*> vce(lastValue, m_selected);
			directNotify(vce);
		}

	private:
		DynamicWindowIG<FocusEventListener>* m_wListBox;
		Label* m_label;
		const std::string m_styleName;
		std::vector<T> m_values;
		T* m_selected;

		const unsigned int m_fontSize;
	};
}
