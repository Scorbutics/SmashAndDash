#pragma once
#include <optional>
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
	CustomEntityManager(CustomEntityManager&&) = default;
	CustomEntityManager& operator=(CustomEntityManager&&) = default;

	ska::EntityId createSkill(const SkillsHolderComponent& shc, unsigned int index);
	virtual ska::EntityId createTrainer(const ska::Point<int> startBlockPos, const unsigned int worldBlockSize) override;
	virtual ska::EntityId createCharacter(const ska::Point<int> startBlockPos, const int id, const unsigned int worldBlockSize, const std::string& name = "") override;	
	~CustomEntityManager() override = default;

	// Moves an entity (pixels) with its physics body, when it has one
	static void place(ska::EntityManager& em, ska::cp::Space& space, ska::EntityId entity, ska::Point<float> position);
	// Where the physics shape of a character (its size) is in its block, from the position of the character (the corner of
	// the block) : centered horizontally, at the bottom of the block (under its feet), 1 pixel away from the block under it
	static ska::Point<int> physicsShapeOffset(ska::Point<int> shapeSize, int blockSize);
	static ska::EntityId createCharacterNG(CustomEntityManager& em, ska::cp::Space& space, const ska::Point<int> startBlockPos, const int id, const unsigned int worldBlockSize, const std::string& name = "");
	// A character moved by forces (inputs, AI movements) : its physics body has a control body, as the trainer
	static ska::EntityId createMovingCharacterNG(CustomEntityManager& em, ska::cp::Space& space, const ska::Point<int> startBlockPos, const int id, const unsigned int worldBlockSize, const std::string& name = "");
	static ska::EntityId createTrainerNG(CustomEntityManager& em, ska::cp::Space& space, const ska::Point<int> startBlockPos, const unsigned int worldBlockSize, const std::string& name = "");
	static void fillCharacter(CustomEntityManager& em, ska::cp::Space& space, ska::EntityId character, float friction = DEFAULT_FRICTION, float rotationFriction = DEFAULT_ROTATION_FRICTION);
	// The trainer walks freely on the map (GridMovementSystem) : its physics shape is its feet (TrainerShape pixels square), within the
	// free cells of a block a character can stand on (ska CollisionProfile::StandCells : 24 x 24 pixels, the middle of
	// the lower half), ex : a door between its frame
	static void fillTrainer(CustomEntityManager& em, ska::cp::Space& space, ska::EntityId trainer);
	static constexpr int TrainerShape = 22;
private:
	static void fill(CustomEntityManager& em, ska::cp::Space& space, ska::EntityId character, float friction, float rotationFriction, std::optional<ska::Point<int>> shapeSize);
	static constexpr auto DEFAULT_FRICTION = 60.F;
	static constexpr auto DEFAULT_ROTATION_FRICTION = 10.F;
};

