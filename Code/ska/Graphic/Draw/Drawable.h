#pragma once

namespace ska {
	class Drawable {
	public:
		Drawable() { }
		virtual ~Drawable() {}

		virtual void display() = 0;

		static bool staticOperatorInf(Drawable* a, Drawable* b) { return *a < *b; }

		bool operator< (Drawable const &id) {
			int p1 = getPriority();
			int p2 = id.getPriority();
			return p1 < p2;
		}
		
		virtual int getPriority2D() const {
			return getPriority();
		}

		virtual int getPriority() const = 0;
		virtual bool isVisible() const = 0;
	};
}