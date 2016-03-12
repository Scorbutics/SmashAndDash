#include "PriorityQueueDrawableContainer.h"


ska::PriorityQueueDrawableContainer::PriorityQueueDrawableContainer()
{
}

void ska::PriorityQueueDrawableContainer::push(Drawable& d) {
	m_data.push(&d);
}

void ska::PriorityQueueDrawableContainer::draw() {
	for (Drawable* d = m_data.top(); !m_data.empty(); m_data.pop()) {
		if (d != NULL) {
			d->display();
		}
	}
}

ska::PriorityQueueDrawableContainer::~PriorityQueueDrawableContainer()
{
}
