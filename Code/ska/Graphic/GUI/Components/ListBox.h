#pragma once
#include <vector>
#include <sstream>
#include <algorithm>
#include "WidgetPanel.h"
#include "Button.h"
#include "Label.h"
#include "../GUI.h"

namespace ska {
	
	template <class T>
	class ListBox : public WidgetPanel<HoverEventListener, ClickEventListener> {
	public:
		ListBox(GUI& g, Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName, const std::string& buttonStyleName, const ska::Rectangle* clip) : 
			WidgetPanel<HoverEventListener, ClickEventListener>(parent, relativePos),
			m_gui(g),
			m_styleName(buttonStyleName),
			m_selected(nullptr) {
				auto button = std::make_unique<Button>(*this, ska::Point<int>(0, 0), placeHolderStyleName, clip, [this] (Widget* tthis, ClickEvent& e) {
					if(e.getState() != MouseEventType::MOUSE_RELEASE) {
						return;
					}
					auto w = m_wListBox;
					auto b = reinterpret_cast<Button&>(*tthis);
					w->move(ska::Rectangle{ getBox().x, getBox().y + b.getBox().h, b.getBox().w, (int)(m_values.size() + 1)*b.getBox().h });
					//w->setPriority(std::numeric_limits<int>()::max());
					w->show(true);
					//w->focus(true);
					//e.setTarget(w);
					resort();
				});
				
				auto wListbox = std::make_unique<DynamicWindowIG<FocusEventListener>>(parent, ska::Rectangle{ getBox().x, getBox().y + button->getBox().h, button->getBox().w, (int)(m_values.size() + 1)*button->getBox().h }, ska::Button::MENU_DEFAULT_THEME_PATH + "listboxmenu");
				wListbox->show(false);
				/*wListbox->addHandler<ClickEventListener>([](Widget* tthis, ClickEvent& e) {
					if(e.getState() == MOUSE_RELEASE) {
						tthis->focus(true);
					}
				});*/

				/*wListbox->addHandler<FocusEventListener>([](Widget* tthis, FocusEvent& e) {
					if(e.getState() == MOUSE_BLUR) {
						tthis->show(false);
					}
				});*/

				setWidth(button->getBox().w);
				setHeight(button->getBox().h);

				/*m_gui.addWindow(std::move(m_wListbox));*/
				
				m_wListBox = addWidget(wListbox);
				m_wListBox->setHeight(getBox().h);
				m_wListBox->setPriority(1);
				addWidget(button)->setPriority(3);
				m_label = addWidget(std::make_unique<Label>(*this, " ", 12, ska::Point<int>(10, 2)));
				m_label->setPriority(2);
				
		}

		void clearValue() {
			m_selected = nullptr;
			m_label->modifyText(" ");
		}

		void load(std::vector<T>&& values) {
			m_values = std::move(values);
			auto& wListbox = *m_wListBox;
			wListbox.clear();
			static unsigned int  buttonHeight = 12;
			size_t index =0;

			auto b1 = std::make_unique<Button>(wListbox, ska::Point<int>(0, buttonHeight * (int)index), m_styleName, nullptr, [&](Widget* tthis, ClickEvent& e) {
				if (e.getState() != MouseEventType::MOUSE_RELEASE) {
					return;
				}
				auto button = reinterpret_cast<Button*> (tthis);
				clearValue();
				button->getParent()->show(false);
			});

			wListbox.addWidget(std::make_unique<Label>(wListbox, " " , 12, 
				ska::Point<int>(10, buttonHeight * (int)index + 2))
				);
			wListbox.addWidget(b1);
			index++;

			for (auto& v : m_values) {
				auto b = std::make_unique<Button>(wListbox, ska::Point<int>(0, buttonHeight * (int)index), m_styleName, nullptr, [&] (Widget* tthis, ClickEvent& e) {
					if (e.getState() != MouseEventType::MOUSE_RELEASE) {
						return;
					}
					auto button = reinterpret_cast<Button*> (tthis);
					forceValue(v);
					button->getParent()->show(false);
				});
				

				std::stringstream ss;
				ss << v;
				auto str = ss.str();
				wListbox.addWidget(std::make_unique<Label>(wListbox, str.empty() ? " " : str, 12, ska::Point<int>(10, buttonHeight * (int)index + 2)));
				wListbox.addWidget(b);

				index++;
			}
			wListbox.setHeight(buttonHeight * ((int)m_values.size() + 1));
			//wListbox.resort();
		}

		T* getValue() const {
			return m_selected;
		}

		void forceValue(T& v) {
			m_selected = &v;
			std::stringstream ss;
			ss << *m_selected;
			auto str = ss.str();
			m_label->modifyText(str.empty() ? " " : str);
		}

	private:
		DynamicWindowIG<FocusEventListener>* m_wListBox;
		Label* m_label;
		const std::string& m_styleName;
		std::vector<T> m_values;
		T* m_selected;
		GUI& m_gui;
	};
}