#pragma once
#include "PrefabEntityManager.h"
#include "Physic/Space.h"

namespace ska {
	class IniReader;
}

class SkillsHolderComponent;

class CustomEntityManager : 
	public ska::PrefabEntityManager {

public:
	CustomEntityManager(ska::GameEventDispatcher& ged);
	ska::EntityId createSkill(const SkillsHolderComponent& shc, unsigned int index);
	virtual ska::EntityId createTrainer(const ska::Point<int> startBlockPos, const unsigned int worldBlockSize) override;
	virtual ska::EntityId createCharacter(const ska::Point<int> startBlockPos, const int id, const unsigned int worldBlockSize, const std::string& name = "") override;
	~CustomEntityManager() override = default;

	static ska::EntityId createCharacterNG(CustomEntityManager& em, ska::cp::Space& space, const ska::Point<int> startBlockPos, const int id, const unsigned int worldBlockSize, const std::string& name = "");
	static ska::EntityId createTrainerNG(CustomEntityManager& em, ska::cp::Space& space, const ska::Point<int> startBlockPos, const unsigned int worldBlockSize, const std::string& name = "");
};

