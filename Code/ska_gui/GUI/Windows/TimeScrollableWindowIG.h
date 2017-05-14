#pragma once
#include "DynamicWindowIG.h"
#include "../Utils/TimeObserver.h"
#include "../Utils/TimeObservable.h"
#include "../Events/TimeEventListener.h"
#include "Task/WorkNode.h"

namespace ska {

	template <class ...HL>
	class TimeScrollableWindowIG :
		public DynamicWindowIG<TimeEventListener, HL...>,
		public TimeObserver {
	public:

		TimeScrollableWindowIG(Widget& parent, const Rectangle& box, const std::string& styleName) :
			DynamicWindowIG<TimeEventListener, HL...>(parent, box, styleName),
			Observer<TimeEvent>(std::bind(&TimeScrollableWindowIG::timeEvent, this, std::placeholders::_1)), m_moving(false),
			m_timeObservable(nullptr) {

			HandledWidget<ClickEventListener, HoverEventListener, TimeEventListener, HL...>::template addHandler<TimeEventListener>([&](Widget*, TimeEvent&) {
				refreshInternal();
			});
		}

		TimeScrollableWindowIG(TimeObservable* timeObservable, MouseObservable* guiObservable, KeyObservable* keyObservable, const Rectangle& box, const std::string& styleName) :
			DynamicWindowIG<TimeEventListener, HL...>(guiObservable, keyObservable, box, styleName),
			Observer<TimeEvent>(std::bind(&TimeScrollableWindowIG::timeEvent, this, std::placeholders::_1)), m_moving(false),
			m_timeObservable(timeObservable) {

			if (m_timeObservable != nullptr) {
				m_timeObservable->addObserver(*this);
			}

			HandledWidget<ClickEventListener, HoverEventListener, TimeEventListener, HL...>::template addHandler<TimeEventListener>([&](Widget*, TimeEvent&) {
				refreshInternal();
			});
		}

		TimeScrollableWindowIG<HL...>& operator=(const TimeScrollableWindowIG<HL...>&) = delete;


		virtual WorkNode<TimeScrollableWindowIG<HL...>>& scrollTo(const Point<int>& relativeTargetPos, unsigned int steps) {
			/* If we're already scrolling, do not scroll. First, wait to finish the current scroll. */
			if (m_moving) {
				return m_scrollCallback;
			}

			m_destinationPos = relativeTargetPos;
			auto speed = steps == 0 ? 1.0 : (1.0 / steps);

			m_moving = true;

			const auto& pos = this->getRelativePosition();
			m_slope.y = (m_destinationPos.y - pos.y) * speed;
			m_slope.x = (m_destinationPos.x - pos.x) * speed;

			if (NumberUtils::isLowValue(m_slope.x, 1.0) && m_destinationPos.x != pos.x) {
				m_slope.x = NumberUtils::calculateSlopeSign(m_destinationPos.x, pos.x);
			}

			if (NumberUtils::isLowValue(m_slope.y, 1.0) && m_destinationPos.y != pos.y) {
				m_slope.y = NumberUtils::calculateSlopeSign(m_destinationPos.y, pos.y);
			}
			return m_scrollCallback;
		}

		void scrollStop() {
			m_moving = false;
			m_destinationPos = this->getRelativePosition();
		}


		virtual ~TimeScrollableWindowIG(){
			if(m_timeObservable != nullptr) {
				m_timeObservable->removeObserver(*this);
			}
		}

	protected:
		bool isMoving() const {
			return m_moving;
		}

	private:
		bool timeEvent(TimeEvent& e) {
			return this->notify(e);
		}

		bool refreshInternal() {
			if (!m_moving) {
                m_scrollCallback.executeAndPop(*this);
				return false;
			}

			const auto& pos = this->getRelativePosition();

			auto nextPos = pos;
			auto distx = static_cast<unsigned int>((static_cast<int>(m_slope.x) != 0) ? (m_destinationPos.x - pos.x) * (m_destinationPos.x - pos.x) : 0);
			auto xdone = distx == 0;
			if (m_slope.x * m_slope.x >= distx) {
				nextPos.x = m_destinationPos.x;
				m_slope.x = 0;
			} else {
				nextPos.x += static_cast<int>(m_slope.x);
			}

			auto disty = static_cast<unsigned int>((static_cast<int>(m_slope.y) != 0) ? (m_destinationPos.y - pos.y) * (m_destinationPos.y - pos.y) : 0);
			auto ydone = disty == 0;
			if (m_slope.y * m_slope.y >= disty) {
				nextPos.y = m_destinationPos.y;
				m_slope.y = 0;
			} else {
				nextPos.y += static_cast<int>(m_slope.y);
			}

			if (xdone && ydone) {
				m_moving = false;
			}

			this->move(nextPos);
			return true;
		}

		Point<int> m_destinationPos;
		Point<double> m_slope;
		bool m_moving;

		WorkNode<TimeScrollableWindowIG<HL...>> m_scrollCallback;

		TimeObservable *const m_timeObservable;

	};
}
