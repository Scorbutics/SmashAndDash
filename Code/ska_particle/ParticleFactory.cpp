#include <cassert>
#include "ParticleFactory.h"
#include "ParticleGroup.h"
#include "ParticleBuilder.h"
#include <iostream>

ska::ParticleFactory::ParticleFactory(ParticleBuilder& builder):
    m_group(nullptr),
    m_lastActiveIndex(0),
    m_needsCounterUpdate(false),
    m_builder(builder) {}

ska::ParticleBuilder& ska::ParticleFactory::prepareNextParticles(ParticleGroup& group, unsigned int density, unsigned int ellapsedTime)  {
	m_group = &group;

	if(hasAvailableNextParticles(ellapsedTime)) {
        group.addIndex(density);
        //std::cout << "Next particle range available : " << m_lastActiveIndex << " - " << group.activeIndex << std::endl;
        m_builder.target(group, m_lastActiveIndex, group.activeIndex);
        for(auto i = m_lastActiveIndex; i < group.activeIndex; i++) {
            //std::cout << "Lifetime reset for particle " << i << std::endl;
            group.lifetime[i] = 0;
        }
        m_needsCounterUpdate = true;

        if(!m_builder.isEmpty()) {
            std::cout << "Generating from " << m_lastActiveIndex << " to " << group.activeIndex<< std::endl;
        }
	} else {
	    m_builder.target(group, 0, 0);
	}
	return m_builder;
}

bool ska::ParticleFactory::hasAvailableNextParticles(unsigned int ellapsedTime) const {
    if(m_group->timeSinceLastGeneration >= m_group->generationDelay) {
       m_group->timeSinceLastGeneration = 0;
       return true;
    }
    m_group->timeSinceLastGeneration += ellapsedTime;
    return false;
}

void ska::ParticleFactory::updateCurrentActiveCounter() {
    if(m_needsCounterUpdate) {
        m_needsCounterUpdate = false;
        m_lastActiveIndex = m_group->activeIndex;
    }
}
