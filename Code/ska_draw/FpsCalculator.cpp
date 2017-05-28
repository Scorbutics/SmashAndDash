#include <iostream>
#include "FpsCalculator.h"

ska::FpsCalculator::FpsCalculator() : m_tickindex(0), m_ticksum(0) {
	m_ticklist.fill(0);
}

void ska::FpsCalculator::setDisplayPriority(int p) {
	m_renderable.setPriority(p);
}

const ska::FpsRenderable& ska::FpsCalculator::getRenderable() const {
	return m_renderable;
}

double ska::FpsCalculator::calculate(long newtick) {
	/* subtract value falling off */
	m_ticksum -= m_ticklist[m_tickindex];

	/* add new value */
	m_ticksum += newtick;

	/* save new value so it can be subtracted later */
	m_ticklist[m_tickindex] = newtick;

	if (++m_tickindex == MAXSAMPLES) {
		m_tickindex = 0;
	}

	const auto average = (static_cast<double>(m_ticksum) / MAXSAMPLES);
	m_renderable.update((average == 0.0) ? -1 : (1000 / average));
	
	return average;

}
