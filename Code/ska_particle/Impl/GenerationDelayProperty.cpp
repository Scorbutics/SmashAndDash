#include <iostream>
#include "GenerationDelayProperty.h"
#include "../ParticleGroup.h"

void ska::GenerationDelayProperty::applyProperty(ParticleGroup& group, unsigned int generationDelay) {
	std::cout << "Setting Generation delay : " <<  generationDelay << std::endl;
	group.generationDelay = generationDelay;
}
