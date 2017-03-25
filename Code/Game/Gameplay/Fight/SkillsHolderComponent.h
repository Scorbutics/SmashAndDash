#pragma once
#include "ECS/Component.h"
#include <array>
#include "SkillDescriptor.h"

class SkillsHolderComponent : public ska::Component {
public:
	std::array<SkillDescriptor, 4> skills;
	std::array<unsigned int, 4> remaingCDs;
};
