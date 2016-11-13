#pragma once
#include "WindowIG.h"

namespace ska {

	template <class ...HL>
	class TimeScrollableWindowIG : public WindowIG<HL...> {
	public:

		TimeScrollableWindowIG(ska::Widget& parent, const ska::Rectangle& box, bool drawStyle) : 
			WindowIG<HL...>(parent, box, drawStyle) {
		}
		
		TimeScrollableWindowIG(const ska::Rectangle& box, bool drawStyle) :
			WindowIG<HL...>(box, drawStyle) {
		}

		bool refresh() {
			if (!m_moving) {
				return false;
			}

			const auto& pos = getRelativePosition();

			bool xdone = false;
			bool ydone = false;
			auto nextPos = pos;
			unsigned int distx = (m_slope.x != 0.0) ? (m_destinationPos.x - pos.x) * (m_destinationPos.x - pos.x) : 0;
			xdone = distx == 0;
			if (m_slope.x*m_slope.x >= distx) {
				nextPos.x = m_destinationPos.x;
				m_slope.x = 0;
			}
			else {
				nextPos.x += (int) m_slope.x;
			}

			unsigned int disty = (m_slope.y != 0.0) ? (m_destinationPos.y - pos.y) * (m_destinationPos.y - pos.y) : 0;
			ydone = disty == 0;
			if (m_slope.y*m_slope.y >= disty) {
				nextPos.y = m_destinationPos.y;
				m_slope.y = 0;
			}
			else {
				nextPos.y += (int) m_slope.y;
			}

			if (xdone && ydone) {
				m_moving = false;
			}

			move(nextPos);
			return true;
		}

		bool scrollTo(const ska::Point<int>& relativeTargetPos, unsigned int steps) {
			/* If we're already scrolling, do not scroll. First, wait to finish the current scroll. */
			if (m_moving) {
				return false;
			}

			m_destinationPos = relativeTargetPos;
			double speed = steps == 0 ? 1.0 : (1.0 / steps);

			m_moving = true;

			const auto& pos = getRelativePosition();
			m_slope.y = (m_destinationPos.y - pos.y) * speed;
			m_slope.x = (m_destinationPos.x - pos.x) * speed;

			if (ska::NumberUtils::isLowValue(m_slope.x, 1.0) && m_destinationPos.x != pos.x) {
				m_slope.x = ska::NumberUtils::calculateSlopeSign(m_destinationPos.x, pos.x);
			}

			if (ska::NumberUtils::isLowValue(m_slope.y, 1.0) && m_destinationPos.y != pos.y) {
				m_slope.y = ska::NumberUtils::calculateSlopeSign(m_destinationPos.y, pos.y);
			}
			return true;
		}

		void scrollStop() {
			m_moving = false;
			m_destinationPos = getRelativePosition();
		}


		virtual ~TimeScrollableWindowIG() = default;
	
	protected:
		bool isMoving() const {
			return m_moving;
		}

	private:
		Point<int> m_destinationPos;
		ska::Point<double> m_slope;
		bool m_moving;

	};
}