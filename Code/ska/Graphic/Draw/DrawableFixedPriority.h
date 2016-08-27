#pragma once
#include "Drawable.h"

namespace ska {
	class DrawableFixedPriority : virtual public Drawable {
	public:
		DrawableFixedPriority() {
			m_priority = 0;
		}

		virtual void setPriority(const int p) {
			m_priority = p;
		}

		virtual ~DrawableFixedPriority() {}

		virtual int getPriority() const override {
			return m_priority;
		}

	private:
		int m_priority;
	};
}