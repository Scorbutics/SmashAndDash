#pragma once
#include "TimeScrollableWindowIG.h"

#include "./Components/MouseObserver.h"
#include "./Components/MouseObservable.h"
#include "./Components/KeyObserver.h"
#include "./Components/KeyObservable.h"
#include "./Components/Hoverable.h"

#include "./Components/ClickEventListener.h"
#include "./Components/HoverEventListener.h"

namespace ska {

	class KeyEvent;
	class HoverEvent;
	class ClickEvent;

	template <class ...HL>
	class DynamicWindowIG :
		public TimeScrollableWindowIG<ClickEventListener, HoverEventListener, HL...>,
		public HoverStateController<DynamicWindowIG<HL...>>,
		public MouseObserver,
		public KeyObserver {

	public:
		DynamicWindowIG(Widget& parent, const Rectangle& box, const std::string& styleName) :
			TimeScrollableWindowIG<ClickEventListener, HoverEventListener, HL...>(parent, nullptr, box, styleName),
			MouseObserver(std::bind(&DynamicWindowIG<HL...>::hoverEvent, this, std::placeholders::_1), std::bind(&DynamicWindowIG<HL...>::clickEvent, this, std::placeholders::_1)),
			KeyObserver(std::bind(&DynamicWindowIG<HL...>::keyEvent, this, std::placeholders::_1)),
			m_guiObservable(nullptr),
			m_keyObservable(nullptr) {
				this->move(box);
				this->setWidth(box.w);
				this->setHeight(box.h);
		}

		DynamicWindowIG(TimeObservable* timeObservable, MouseObservable* guiObservable, KeyObservable* keyObservable, const Rectangle& box, const std::string& styleName) :
			TimeScrollableWindowIG<ClickEventListener, HoverEventListener, HL...>(timeObservable, box, styleName),
			MouseObserver(std::bind(&DynamicWindowIG<HL...>::hoverEvent, this, std::placeholders::_1), std::bind(&DynamicWindowIG<HL...>::clickEvent, this, std::placeholders::_1)),
			KeyObserver(std::bind(&DynamicWindowIG<HL...>::keyEvent, this, std::placeholders::_1)),
			m_guiObservable(guiObservable),
			m_keyObservable(keyObservable) {

			m_guiObservable->HoverObservable::addObserver(*this);
			m_guiObservable->ClickObservable::addObserver(*this);
			m_keyObservable->addObserver(*this);
		}

		DynamicWindowIG<HL...>& operator=(const DynamicWindowIG<HL...>&) = delete;

		virtual ~DynamicWindowIG() {
			if (m_guiObservable != nullptr) {
				m_guiObservable->HoverObservable::removeObserver(*this);
				m_guiObservable->ClickObservable::removeObserver(*this);
			}

			if (m_keyObservable != nullptr) {
				m_keyObservable->removeObserver(*this);
			}
		}

	protected:
		bool keyEvent(KeyEvent& e) {
			return this->notify(e);
		}

		bool hoverEvent(HoverEvent& e) {
			return this->notify(e);
		}

		bool clickEvent(ClickEvent& e) {
			return this->notify(e);
		}

	private:
		MouseObservable *const m_guiObservable;
		KeyObservable *const m_keyObservable;
	};
}
