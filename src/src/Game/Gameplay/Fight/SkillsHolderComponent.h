#pragma once
#include <array>
#include "SkillDescriptor.h"

class SkillsHolderComponent {
public:
	std::array<SkillDescriptor, 4> skills;
	std::array<unsigned int, 4> remaingCDs;
};
