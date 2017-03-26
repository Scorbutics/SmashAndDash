#pragma once
#include <memory>

namespace ska {
	class Drawable {
	public:
		Drawable() { }
		virtual ~Drawable() {}

		virtual void display() const = 0;

		static bool staticOperatorInf(const Drawable* a, const Drawable* b) {
			int p1 = a->getPriority();
			int p2 = b->getPriority();
			return p1 < p2;
		}

		bool operator< (const Drawable &id) {
			int p1 = getPriority();
			int p2 = id.getPriority();
			return p1 < p2;
		}
		
		static bool staticOperatorSup(const Drawable* a, const Drawable* b) {
			int p1 = a->getPriority();
			int p2 = b->getPriority();
			return p1 > p2;
		}

		bool operator> (const Drawable &id) {
			int p1 = getPriority();
			int p2 = id.getPriority();
			return p1 > p2;
		}

		virtual int getPriority2D() const {
			return getPriority();
		}

		virtual int getPriority() const = 0;
		virtual bool isVisible() const = 0;
	};
}
