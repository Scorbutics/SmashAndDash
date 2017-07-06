#include <cassert>
#include "ParticleFactory.h"
#include "ParticleGroup.h"

ska::ParticleFactory::ParticleFactory(): m_group(nullptr), m_lastActiveIndex(0){}

ska::ParticleBuilder& ska::ParticleFactory::createNextParticles(ParticleGroup& group, unsigned int density) {
	m_group = &group;
	group.addIndex(density);

	m_builder.target(group, m_lastActiveIndex, group.activeIndex);

	for(auto i = m_lastActiveIndex; i < group.activeIndex; i++) {
		group.lifetime[i] = 0;
	}
	return m_builder;
}

void ska::ParticleFactory::updateCurrentActiveCounter() {
	m_lastActiveIndex = m_group->activeIndex;
}

ska::ParticleBuilder::ParticleBuilder() :
	m_group(nullptr),
	m_indexStart(0),
	m_indexEnd(0) {
}

void ska::ParticleBuilder::target(ParticleGroup& group, std::size_t particleIndexStart, std::size_t particleIndexEnd) {
	m_group = &group;
	m_indexStart = particleIndexStart;
	m_indexEnd = particleIndexEnd;
}

const ska::ParticleBuilder& ska::ParticleBuilder::setAcceleration(Point<float> acceleration) const {
	assert(m_group != nullptr);
	for (auto i = m_indexStart; i < m_indexEnd; i++) {
		m_group->physics[i].acceleration = acceleration;
	}
	return *this;
}

const ska::ParticleBuilder& ska::ParticleBuilder::setVelocity(Point<float> velocity) const {
	assert(m_group != nullptr);
	for (auto i = m_indexStart; i < m_indexEnd; i++) {
		m_group->physics[i].velocity = velocity;
	}
	return *this;
}

const ska::ParticleBuilder& ska::ParticleBuilder::setPosition(Point<float> position) const {
	assert(m_group != nullptr);
	for (auto i = m_indexStart; i < m_indexEnd; i++) {
		m_group->pos[i] = position;
	}
	return *this;
}

const ska::ParticleBuilder& ska::ParticleBuilder::setRandomPosition(Point<float> position, Point<int> maxDistance) const {
	assert(m_group != nullptr);
	for (auto i = m_indexStart; i < m_indexEnd; i++) {
		m_group->pos[i] = position + Point<int>(
			NumberUtils::random(- maxDistance.x, maxDistance.x),
			NumberUtils::random(- maxDistance.y, maxDistance.y));
	}
	return *this;
}

const ska::ParticleBuilder& ska::ParticleBuilder::setStartColor(const Color& c) const{
	assert(m_group != nullptr);
	for (auto i = m_indexStart; i < m_indexEnd; i++) {
		m_group->startColor[i] = c;
	}
	return *this;
}

const ska::ParticleBuilder& ska::ParticleBuilder::setColor(const Color& c) const {
    assert(m_group != nullptr);
	for (auto i = m_indexStart; i < m_indexEnd; i++) {
		m_group->color[i] = c;
	}
	return *this;
}

const ska::ParticleBuilder& ska::ParticleBuilder::setEndColor(const Color& c) const {
	assert(m_group != nullptr);
	for (auto i = m_indexStart; i < m_indexEnd; i++) {
		m_group->endColor[i] = c;
	}
	return *this;
}
