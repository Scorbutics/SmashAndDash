#pragma once
#include "../World/WorldState.h"


class SkillDescriptor;

namespace ska {
	class IniReader;
}

class SkillsHolderComponent;

class SkillFactory {
public:
	static constexpr auto BasicAttack = "./Data/Skills/charge.ini";

	SkillFactory(WorldState& ws, unsigned int level);
	// Its skills : "moveset" (their indexes in its data : a Pokemon of the team), else those of its level
	void loadSkills(const ska::IniReader& reader, const ska::EntityId m_pokemonId, SkillsHolderComponent& shc, const std::vector<int>* moveset = nullptr) const;
	~SkillFactory() = default;

private:
	void createSkill(SkillDescriptor& sd, const std::string& skillPath) const;

	WorldState& m_worldState;
	unsigned int m_level;
};
