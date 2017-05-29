#pragma once
#include "GUI/Events/TimeEventListener.h"
#include "FpsCalculator.h"
#include "GUI/Events/KeyEventListener.h"
#include "../../Gameplay/Fight/SkillDescriptor.h"
#include "GUI/Windows/DynamicWindowIG.h"
#include "GUI/Components/Concrete/ButtonSprite.h"

class SkillsBar : public ska::DynamicWindowIG<ska::TimeEventListener, ska::KeyEventListener> {
public:
	SkillsBar(ska::Widget& parent, const std::array<SkillDescriptor, 4>& skills);
	virtual ~SkillsBar() = default;

private:
	std::vector<ska::ButtonSprite*> m_skillsHolder;
};
