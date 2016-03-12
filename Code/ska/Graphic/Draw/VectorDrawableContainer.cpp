#include <algorithm>
#include "VectorDrawableContainer.h"


ska::VectorDrawableContainer::VectorDrawableContainer()
{
}

void ska::VectorDrawableContainer::push(Drawable& d) {
	m_data.push_back(&d);
}

void ska::VectorDrawableContainer::draw() {
	std::sort(m_data.begin(), m_data.end(), Drawable::staticOperatorInf);
	for (Drawable* d : m_data) {
		if (d != NULL) {
			d->display();
		}
	}
}

ska::VectorDrawableContainer::~VectorDrawableContainer()
{
}
