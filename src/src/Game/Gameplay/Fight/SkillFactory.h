#pragma once
#include "../World/WorldState.h"


class SkillDescriptor;

namespace ska {
	class IniReader;
}

class SkillsHolderComponent;

class SkillFactory {
public:
	SkillFactory(WorldState& ws, unsigned int level);
	void loadSkills(const ska::IniReader& reader, const ska::EntityId m_pokemonId, SkillsHolderComponent& shc) const;
	~SkillFactory() = default;

private:
	void createSkill(SkillDescriptor& sd, const std::string& skillPath) const;

	WorldState& m_worldState;
	unsigned int m_level;
};
