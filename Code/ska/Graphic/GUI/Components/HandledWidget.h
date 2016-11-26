#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>
#include <bitset>
#include <tuple>

#include "Widget.h"
#include "IWidgetEvent.h"
#include "WidgetMaskHelper.h"

#define SKA_GUI_MAX_WIDGET_EVENT_LISTENER 16

namespace ska {

	using ListenerMask = std::bitset<SKA_GUI_MAX_WIDGET_EVENT_LISTENER>;
	using MaskUnit = unsigned int;

	/* Dummy indicator class (for compile time class comparisons) */
	class IHandledWidget {
	public:
		virtual	bool isMaskEmpty() const = 0;
	};

	template <class ...HL>
	class HandledWidget : public Widget, public IHandledWidget {
	public:
		HandledWidget() :
			m_currentMaskIndex(sizeof ...(HL)-1),
			m_handlers(std::make_tuple<HL...>(instantiateHandler<HL>()...)) {
			/* Bracket initializer trick */
			int _[] = { 0, (buildMask<HL>(), 0)... };
			(void)_;
		}

		HandledWidget(Widget& parent)  :
			Widget(parent),
			m_currentMaskIndex(sizeof ...(HL)-1),
			m_handlers(std::make_tuple<HL...>(instantiateHandler<HL>()...)) {
			/* Bracket initializer trick */
			int _[] = { 0, (buildMask<HL>(), 0)... };
			(void)_;
		}

		HandledWidget(Widget& parent, Point<int>& position) : Widget(parent, position),
			m_currentMaskIndex(sizeof ...(HL)-1),
			m_handlers(std::make_tuple<HL...>(instantiateHandler<HL>()...)) {
			/* Bracket initializer trick */
			int _[] = { 0, (buildMask<HL>(), 0)... };
			(void)_;
		}

		bool notify(IWidgetEvent& e) override {
			if (!isVisible()) {
				return false;
			}
			if (m_mask[e.getMask()]) {
				auto maskIndex = m_maskHandlerIndexes[e.getMask()];
				HandlerNotifier hn(*this, e);
				meta::visit_element_at_index(m_handlers, maskIndex, hn);
				return hn.hasBeenNotified();
			}
			return false;
		}

		template<class L, class EH>
		void addHandler(const EH& eh) {
			meta::get<L>(m_handlers).addHandler(eh);
		}

		virtual	bool isMaskEmpty() const override {
			return m_mask.none();
		}

		virtual ~HandledWidget() = default;

		template<class L, class EH>
		void addHeadHandler(const EH& eh) {
			meta::get<L>(m_handlers).addHeadHandler(eh);
		}

	private:
		class HandlerNotifier {
		public:
			HandlerNotifier(HandledWidget<HL...>& tthis, IWidgetEvent& e) : m_event(e), m_this(tthis), m_result(false) {
			}

			template<class T>
			void operator() (T&& t) {
				m_result |= t.notifyGeneric(m_this, m_event);
			}

			bool hasBeenNotified() const {
				return m_result;
			}

		private:
			IWidgetEvent& m_event;
			HandledWidget<HL...>& m_this;
			bool m_result;

		};

		int m_currentMaskIndex;
		std::unordered_map<MaskUnit, int> m_maskHandlerIndexes;
		ListenerMask m_mask;
		std::tuple<HL...> m_handlers;


		template<class T>
		T instantiateHandler() {
			const MaskUnit& mu = WidgetMaskHelper::template getMask<T>();
			m_maskHandlerIndexes[mu] = m_currentMaskIndex;
			m_currentMaskIndex--;
			return T((Widget&)*this);
		}

		template <class T>
		void buildMask() {
			unsigned int mask = WidgetMaskHelper::template getMask<T>();
			if (mask >= m_mask.size()) {
				std::cout << "Error" << std::endl;
				throw std::runtime_error("Too many listeners are used. Unable to continue.");
			}
			m_mask[mask] = true;
		}
	};

	template <>
	class HandledWidget<> : public Widget, public IHandledWidget {
	public:
		bool notify(IWidgetEvent& e) override {
			return false;
		}

		virtual	bool isMaskEmpty() const override {
			return true;
		}
	};
}
