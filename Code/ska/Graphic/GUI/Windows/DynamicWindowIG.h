#pragma once
#include "../Components/Hoverable.h"
#include "../Utils/MouseObserver.h"
#include "../Utils/KeyObserver.h"
#include "../Utils/KeyObservable.h"
#include "../Utils/MouseObservable.h"
#include "../Events/ClickEventListener.h"
#include "WindowIG.h"

namespace ska {

	class KeyEvent;
	class HoverEvent;
	class ClickEvent;

	template <class ...HL>
	class DynamicWindowIG :
		public WindowIG<ClickEventListener, HoverEventListener, HL...>,
		public HoverStateController<DynamicWindowIG<HL...>>,
		public MouseObserver,
		public KeyObserver {

	public:
		DynamicWindowIG(Widget& parent, const Rectangle& box, const std::string& styleName) :
			WindowIG<ClickEventListener, HoverEventListener, HL...>(parent, box, styleName),
			MouseObserver(std::bind(&DynamicWindowIG<HL...>::hoverEvent, this, std::placeholders::_1), std::bind(&DynamicWindowIG<HL...>::clickEvent, this, std::placeholders::_1)),
			KeyObserver(std::bind(&DynamicWindowIG<HL...>::keyEvent, this, std::placeholders::_1)),
			m_guiObservable(nullptr),
			m_keyObservable(nullptr) {
				this->move(box);
				this->setWidth(box.w);
				this->setHeight(box.h);
		}

		DynamicWindowIG(MouseObservable* guiObservable, KeyObservable* keyObservable, const Rectangle& box, const std::string& styleName) :
			WindowIG<ClickEventListener, HoverEventListener, HL...>(box, styleName),
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
