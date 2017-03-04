#pragma once

#include "ValueChangedEvent.h"
#include "ValueChangedEventListener.h"
#include "HoverEventListener.h"
#include "ClickEventListener.h"

#include "Label.h"
#include "RadioButton.h"
#include "RadioButtonGroup.h"
#include "WidgetPanel.h"

namespace ska {
	template <class T>
	class RadioButtonList : public WidgetPanel<ValueChangedEventListener<T*>, HoverEventListener, ClickEventListener> {
	public:
		RadioButtonList(Widget& parent, Point<int> relativePos, const std::string& placeHolderStyleName) :
			WidgetPanel<ValueChangedEventListener<T*>, HoverEventListener, ClickEventListener>(parent, relativePos),
			m_styleName(placeHolderStyleName),
			m_group(std::shared_ptr<RadioButtonGroup>(new RadioButtonGroup())),
			m_value(nullptr),
			m_xpadding(4),
			m_ypadding(4),
			m_fontSize(11) {

		}
		
		void load(std::vector<T>&& values) {

			Point<int> radioCursor(m_xpadding / 2, m_ypadding / 2);
			Point<int> labelCursor(m_xpadding / 2, m_ypadding);
			unsigned int radioHeight = 0;

			
			for (auto& v : values) {
				auto fradio = std::unique_ptr<RadioButton>(new RadioButton(*this, m_group, radioCursor, m_styleName));
				auto radio = addWidget(fradio);

				radio->addHandler<ValueChangedEventListener<bool>>([this](Widget* tthis, ValueChangedEvent<bool>& e) {
					if (e.getValue()) {
						const auto oldValue = m_value;
						m_value = &m_mappedValues[static_cast<RadioButton*>(tthis)];

						ValueChangedEvent<T*> vce(oldValue, m_value);
						directNotify(vce);
					}
				});

				if (radioHeight == 0) {
					radioHeight = radio->getBox().h;
				}

				if (labelCursor.x < (int)(radio->getBox().w + m_xpadding)) {
					labelCursor.x = radio->getBox().w + m_xpadding;
				}

				radioCursor.y += radioHeight + m_ypadding;
				m_mappedValues[static_cast<RadioButton*>(radio)] = v;
			}

			for (auto& v : values) {
				std::stringstream ss;
				ss << v;
				const auto& str = ss.str();
				addWidget(std::unique_ptr<Label>(new Label(*this, str.empty() ? " " : str, m_fontSize, labelCursor)));
				labelCursor.y += radioHeight + m_ypadding;
			}

		}

		virtual ~RadioButtonList() = default;

	private:
		std::shared_ptr<RadioButtonGroup> m_group;
		T* m_value;
		std::unordered_map<RadioButton*, T> m_mappedValues;
		const std::string m_styleName;
		
		const unsigned int m_xpadding;
		const unsigned int m_ypadding;
		const unsigned int m_fontSize;
	};
}