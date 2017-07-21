#include <cassert>
#include "ParticleBuilder.h"
#include "ParticleGroup.h"

ska::ParticleBuilder::ParticleBuilder() :
	m_group(nullptr),
	m_indexStart(0),
	m_indexEnd(0) {
}

bool ska::ParticleBuilder::isEmpty() const {
    return m_indexStart >= m_indexEnd || m_group == nullptr;
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

const ska::ParticleBuilder& ska::ParticleBuilder::setVelocity(PolarPoint<float> velocity, float spreading, unsigned int slices) const {
	assert(m_group != nullptr);
	if (slices == 0) {
		for (auto i = m_indexStart; i < m_indexEnd; i++) {
			m_group->physics[i].velocity = ska::Point<float>::cartesian(velocity.radius, velocity.angle + ska::NumberUtils::random(-spreading, spreading));
		}
	} else {
		static const auto factor = 10000.F;
		for (auto i = m_indexStart; i < m_indexEnd; i++) {
			const auto factoredSpreading = 2 * spreading * factor;
			const auto currentSlice = slices == 1 ? 1 : ska::NumberUtils::random(1, slices);
			const auto spreadingAngle = ((factoredSpreading / slices) * currentSlice) / factor;
			m_group->physics[i].velocity = ska::Point<float>::cartesian(velocity.radius, velocity.angle + spreadingAngle);
		}
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
		m_group->pos[i] = position + Point<float>(
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
