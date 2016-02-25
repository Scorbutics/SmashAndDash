#include <algorithm>
#include "VectorDrawableContainer.h"


VectorDrawableContainer::VectorDrawableContainer()
{
}

void VectorDrawableContainer::push(Drawable& d) {
	m_data.push_back(&d);
}

void VectorDrawableContainer::draw() {
	std::sort(m_data.begin(), m_data.end(), Drawable::staticOperatorInf);
	for (Drawable* d : m_data) {
		if (d != NULL) {
			d->display();
		}
	}
}

VectorDrawableContainer::~VectorDrawableContainer()
{
}
