#pragma once
#include "WindowIG.h"
#include "Components/TimeEventListener.h"
#include "Components/TimeObserver.h"
#include "Components/TimeObservable.h"

namespace ska {

	template <class ...HL>
	class TimeScrollableWindowIG : 
		public WindowIG<TimeEventListener, HL...>,
		public TimeObserver {
	public:

		TimeScrollableWindowIG(Widget& parent, TimeObservable* timeObservable, const Rectangle& box, const std::string& styleName) :
			WindowIG<TimeEventListener, HL...>(parent, box, styleName),
			Observer<TimeEvent>(std::bind(&TimeScrollableWindowIG::timeEvent, this, std::placeholders::_1)), m_moving(false), m_timeObservable(timeObservable) {
			if (m_timeObservable != nullptr) {
				m_timeObservable->addObserver(*this);
			}

			HandledWidget<TimeEventListener, HL...>::template addHandler<TimeEventListener>([&](Widget*, TimeEvent&) {
				refresh();
			});
		}

		TimeScrollableWindowIG(TimeObservable* timeObservable, const Rectangle& box, const std::string& styleName) :
			WindowIG<TimeEventListener, HL...>(box, styleName),
			Observer<TimeEvent>(std::bind(&TimeScrollableWindowIG::timeEvent, this, std::placeholders::_1)), m_moving(false), m_timeObservable(timeObservable) {
			if (m_timeObservable != nullptr) {
				m_timeObservable->addObserver(*this);
			}

			HandledWidget<TimeEventListener, HL...>::template addHandler<TimeEventListener>([&](Widget*, TimeEvent&) {
				refresh();
			});
		}

		TimeScrollableWindowIG<HL...>& operator=(const TimeScrollableWindowIG<HL...>&) = delete;

		

		virtual bool scrollTo(const Point<int>& relativeTargetPos, unsigned int steps) {
			/* If we're already scrolling, do not scroll. First, wait to finish the current scroll. */
			if (m_moving) {
				return false;
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
			return true;
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

		bool refresh() {
			if (!m_moving) {
				return false;
			}

			const auto& pos = this->getRelativePosition();

			auto nextPos = pos;
			unsigned int distx = (m_slope.x != 0.0) ? (m_destinationPos.x - pos.x) * (m_destinationPos.x - pos.x) : 0;
			auto xdone = distx == 0;
			if (m_slope.x*m_slope.x >= distx) {
				nextPos.x = m_destinationPos.x;
				m_slope.x = 0;
			}
			else {
				nextPos.x += static_cast<int>(m_slope.x);
			}

			unsigned int disty = (m_slope.y != 0.0) ? (m_destinationPos.y - pos.y) * (m_destinationPos.y - pos.y) : 0;
			auto ydone = disty == 0;
			if (m_slope.y*m_slope.y >= disty) {
				nextPos.y = m_destinationPos.y;
				m_slope.y = 0;
			}
			else {
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
	
		TimeObservable *const m_timeObservable;

	};
}
