#pragma once
#include "ECS/SerializableComponent.h"
#include <array>
#include "SkillDescriptor.h"

class SkillsHolderComponent : public ska::SerializableComponent {
public:
	std::array<SkillDescriptor, 4> skills;
	std::array<unsigned int, 4> remaingCDs;
};
