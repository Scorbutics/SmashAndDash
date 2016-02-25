#pragma once

class Drawable {
public:
	Drawable() { }
	virtual void display() = 0;
	
	static bool staticOperatorInf(Drawable* a, Drawable* b) { return *a < *b; }

	bool operator< (Drawable const &id) {
		int p1 = getPriority();
		int p2 = id.getPriority();
		return p1 < p2;
	}
	virtual ~Drawable() {}

	virtual int getPriority() const = 0;

};