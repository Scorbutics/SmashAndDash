#include <algorithm>
#include "VectorDrawableContainer.h"


ska::VectorDrawableContainer::VectorDrawableContainer()
{
}

void ska::VectorDrawableContainer::push(Drawable& d) {
	m_data.push_back(&d);
}

void ska::VectorDrawableContainer::clear() {
	m_data.clear();
}

void ska::VectorDrawableContainer::reserve(const unsigned int size) {
	m_data.reserve(size);
}

std::vector<ska::Drawable*>::iterator ska::VectorDrawableContainer::begin() {
	return m_data.begin();
}

std::vector<ska::Drawable*>::iterator ska::VectorDrawableContainer::end() {
	return m_data.end();
}

std::vector<ska::Drawable*>::const_iterator ska::VectorDrawableContainer::cbegin() const{
	return m_data.cbegin();
}

std::vector<ska::Drawable*>::const_iterator ska::VectorDrawableContainer::cend() const {
	return m_data.cend();
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
