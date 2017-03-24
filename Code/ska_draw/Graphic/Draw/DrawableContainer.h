#pragma once
#include "DrawableFixedPriority.h"
#include <climits>

namespace ska {
	class DrawableContainer
	{
	public:
		DrawableContainer() { m_topPriority2D = INT_MIN; m_topPriority = INT_MIN; }
		
		void add(const Drawable& d) {
			if (d.isVisible()) {
				auto currentPriority = d.getPriority();
				auto currentPriority2D = d.getPriority2D();
				if (currentPriority2D > m_topPriority2D) {
					m_topPriority2D = currentPriority2D;
				}

				if (currentPriority > m_topPriority) {
					m_topPriority = currentPriority;
				}

				push(d);
			}
		}

		void addHead(DrawableFixedPriority& d) {
			if (d.isVisible()) {
				d.setPriority(++m_topPriority);
				add(d);
			}
		}

		void addHead2D(DrawableFixedPriority& d) {
			if (d.isVisible()) {
				d.setPriority(++m_topPriority2D);
				add(d);
			}
		}

		virtual void draw() = 0;
		virtual void clear() = 0;
		virtual ~DrawableContainer() {}

	protected:
		virtual void push(const Drawable& d) = 0;

	private:
		int m_topPriority2D;
		int m_topPriority;
	};
}

