#pragma once
#include "DrawableFixedPriority.h"
#include <climits>

class DrawableContainer
{
public:
	DrawableContainer() { m_topPriority = INT_MIN; }
	void add(Drawable& d) {
		int currentPriority = d.getPriority();
		if (currentPriority > m_topPriority) {
			m_topPriority = currentPriority;
		}
		push(d);
	}

	void addHead(DrawableFixedPriority& d) {
		d.setPriority(++m_topPriority);
		add(d);
	}

	virtual void draw() = 0;
	virtual ~DrawableContainer() {}

protected:
	virtual void push(Drawable& d) = 0;

private:
	int m_topPriority;
};

