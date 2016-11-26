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
		DynamicWindowIG(Widget& parent, const Rectangle& box, bool drawStyle) :
			TimeScrollableWindowIG<ClickEventListener, HoverEventListener, HL...>(parent, box, drawStyle),
			m_guiObservable(nullptr),
			m_keyObservable(nullptr),
			MouseObserver(std::bind(&ska::DynamicWindowIG<HL...>::hoverEvent, this, std::placeholders::_1), std::bind(&ska::DynamicWindowIG<HL...>::clickEvent, this, std::placeholders::_1)),
			KeyObserver(std::bind(&ska::DynamicWindowIG<HL...>::keyEvent, this, std::placeholders::_1)) {
				this->move(box);
				this->setWidth(box.w);
				this->setHeight(box.h);
		}

		DynamicWindowIG(MouseObservable* guiObservable, KeyObservable* keyObservable, const Rectangle& box, bool drawStyle) :
			TimeScrollableWindowIG<ClickEventListener, HoverEventListener, HL...>(box, drawStyle),
			m_guiObservable(guiObservable),
			m_keyObservable(keyObservable),
			MouseObserver(std::bind(&ska::DynamicWindowIG<HL...>::hoverEvent, this, std::placeholders::_1), std::bind(&ska::DynamicWindowIG<HL...>::clickEvent, this, std::placeholders::_1)),
			KeyObserver(std::bind(&ska::DynamicWindowIG<HL...>::keyEvent, this, std::placeholders::_1)) {

			m_guiObservable->HoverObservable::addObserver(*this);
			m_guiObservable->ClickObservable::addObserver(*this);
			m_keyObservable->addObserver(*this);
		}

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
