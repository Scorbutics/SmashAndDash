#pragma once
#include <queue>

class Drawable {
public:
	Drawable() { m_priority = 0;  }
	virtual void display() = 0;
	
	bool operator< (Drawable const &id) {
		int p1 = m_priority;
		int p2 = id.m_priority;
		return p1 < p2;
	}
	virtual ~Drawable() {}

	void setPriority(int p) {
		m_priority = p;
	}

private:
	int m_priority;
};