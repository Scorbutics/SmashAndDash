#pragma once
#include "../../ska/ECS/Component.h"
#include <array>
#include "SkillDescriptor.h"

class SkillsHolderComponent : public ska::Component {
public:
	std::array<SkillDescriptor, 4> skills;
};