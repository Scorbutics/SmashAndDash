#include "PriorityQueueDrawableContainer.h"


PriorityQueueDrawableContainer::PriorityQueueDrawableContainer()
{
}

void PriorityQueueDrawableContainer::push(Drawable& d) {
	m_data.push(&d);
}

void PriorityQueueDrawableContainer::draw() {
	for (Drawable* d = m_data.top(); !m_data.empty(); m_data.pop()) {
		if (d != NULL) {
			d->display();
		}
	}
}

PriorityQueueDrawableContainer::~PriorityQueueDrawableContainer()
{
}
