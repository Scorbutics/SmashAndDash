#include <algorithm>
#include "VectorDrawableContainer.h"


ska::VectorDrawableContainer::VectorDrawableContainer()
{
}

void ska::VectorDrawableContainer::push(const Drawable& d) {
	m_data.push_back(&d);
}

void ska::VectorDrawableContainer::clear() {
	m_data.clear();
}

void ska::VectorDrawableContainer::reserve(const unsigned int size) {
	m_data.reserve(size);
}

std::vector<const ska::Drawable*>::iterator ska::VectorDrawableContainer::begin() {
	return m_data.begin();
}

std::vector<const ska::Drawable*>::iterator ska::VectorDrawableContainer::end() {
	return m_data.end();
}

std::vector<const ska::Drawable*>::const_iterator ska::VectorDrawableContainer::cbegin() const{
	return m_data.cbegin();
}

std::vector<const ska::Drawable*>::const_iterator ska::VectorDrawableContainer::cend() const {
	return m_data.cend();
}

void ska::VectorDrawableContainer::draw() {
	sort(m_data.begin(), m_data.end(), Drawable::staticOperatorInf);
	for (auto d : m_data) {
			d->display();
	}
}

ska::VectorDrawableContainer::~VectorDrawableContainer()
{
}
