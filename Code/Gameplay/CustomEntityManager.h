#pragma once
#include "../ska/ECS/PrefabEntityManager.h"

namespace ska {
	class IniReader;
}

class SkillsHolderComponent;

class CustomEntityManager : public ska::PrefabEntityManager {
public:
	CustomEntityManager();
	ska::EntityId createSkill(const ska::IniReader& reader, const SkillsHolderComponent& shc, unsigned int index);
	virtual ska::EntityId createTrainer(const ska::Point<int> startPos, const unsigned int worldBlockSize) override;
	virtual ska::EntityId createCharacter(const ska::Point<int> startPos, const int id, const unsigned int worldBlockSize) override;
	virtual ~CustomEntityManager();
};

