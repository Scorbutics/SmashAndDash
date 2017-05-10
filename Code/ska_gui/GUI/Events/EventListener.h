#pragma once
#include <functional>
#include <deque>
#include "StopType.h"

namespace ska {

	class Widget;
	class IWidgetEvent;

	template <class E>
	using EventHandler = std::function<void(Widget*, E&)>;

	template <class E>
	class IEventListener {
	public:
		virtual ~IEventListener() = default;

		bool notifyGeneric(Widget& tthis, IWidgetEvent& e) {
			return notify(tthis, static_cast<E&>(e));
		}
		virtual bool notify(Widget& tthis, E& e) = 0;
	};

	template <class E>
	class EventListener : public IEventListener<E> {
	public:
		EventListener() {
		}

		void addHandler(const EventHandler<E>& h) {
			m_callbacks.push_back(h);
		}

		virtual bool notify(Widget& tthis, E& e) override {
			auto affect = e.affects(tthis);
			if (!affect) {
				return false;
			}

			for (auto& eh : m_callbacks) {
				/* Si un callback indique à l'évènement qu'il est totalement stoppé, on stoppe la chaîne des callbacks*/
				eh(&tthis, e);
				if (e.stopped() == STOP_CALLBACK) {
					e.stopPropagation(NOT_STOPPED);
					break;
				}
			}
			return true;
		}

		void addHeadHandler(const EventHandler<E>& h) {
			m_callbacks.push_front(h);
		}

		virtual ~EventListener() = default;

	private:
		std::deque<EventHandler<E>> m_callbacks;
	};
}
