#include <optional>
#include "Game/__internalConfig/LoggerConfig.h"
#include "CustomEntityManager.h"
#include "./Fight/FightComponent.h"
#include "./Mobs/MobSpawnAreaComponent.h"
#include "Graphic/GraphicComponent.h"
#include "./Fight/SkillComponent.h"
#include "Core/ECS/Basics/Physic/MovementComponent.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Core/ECS/Basics/Physic/ForceComponent.h"
#include "Core/ECS/Basics/Physic/CollidableComponent.h"
#include "Core//Utils/SpritePath.h"
#include "./Fight/SkillsHolderComponent.h"
#include "Base/Values/Numbers/NumberUtils.h"
#include "../Utils/IDs.h"

#include "Physic/BuildHitbox.h"
#include "Core/ECS/Basics/Input/InputComponent.h"

CustomEntityManager::CustomEntityManager(ska::GameEventDispatcher& ged) : 
	ska::PrefabEntityManager(ged) {
}

ska::EntityId CustomEntityManager::createSkill(const SkillsHolderComponent& shc, unsigned int index) {

	SkillComponent sc;
	//sc.noise = reader.getInt("Particle slope_noise");
	const auto& sd = shc.skills[index];
	sc.speed = sd.speed;
	sc.damage = -sd.buffEnemy.hp;
	sc.power = sd.power;
	sc.special = sd.special;
	sc.knockback = sd.knockback;
	sc.range = sd.range;
	sc.spriteId = sd.id;
	sc.skillIndex = index;
	const auto maxNoise = sd.noise;
	sc.noise = ska::NumberUtils::random(maxNoise/2, maxNoise);
	sc.amplitude = sd.amplitude;


	auto skill = createEntity();

	ska::PositionComponent pcSkill;
	ska::GraphicComponent gcSkill;
	gcSkill.desiredPriority = SKILLS_DEFAULT_DISPLAY_PRIORITY;
	gcSkill.animatedSprites.resize(1);
	gcSkill.animatedSprites[0].load(ska::SpritePath::getInstance().getPath(SPRITEBANK_SKILL, sd.id), 2, 2, 2);
	addComponent<ska::MovementComponent>(skill, ska::MovementComponent());
	addComponent<SkillComponent>(skill, std::move(sc));
	addComponent<ska::PositionComponent>(skill, std::move(pcSkill));
	ska::HitboxComponent hc;
	hc.xOffset = gcSkill.animatedSprites[0].getWidth() / 2 - 1;
	hc.yOffset = gcSkill.animatedSprites[0].getHeight() / 2 - 1;
	hc.width = gcSkill.animatedSprites[0].getWidth() / 2;
	hc.height = gcSkill.animatedSprites[0].getHeight() / 2;
	addComponent<ska::ForceComponent>(skill, ska::ForceComponent());
	addComponent<ska::HitboxComponent>(skill, std::move(hc));
	addComponent<ska::GraphicComponent>(skill, std::move(gcSkill));
	addComponent<ska::CollidableComponent>(skill, ska::CollidableComponent());
	return skill;
}

ska::EntityId CustomEntityManager::createTrainer(const ska::Point<int> startBlockPos, const unsigned int worldBlockSize) {
	auto trainer = PrefabEntityManager::createTrainer(startBlockPos, worldBlockSize);
	addComponent<MobSpawnAreaComponent>(trainer, MobSpawnAreaComponent());
	addComponent<FightComponent>(trainer, FightComponent());
	return trainer;
}

ska::EntityId CustomEntityManager::createCharacter(const ska::Point<int> startBlockPos, const int id, const unsigned int worldBlockSize, const std::string& name) {
	return PrefabEntityManager::createCharacter(startBlockPos, id, worldBlockSize, name);
}

namespace {
	// The physics shape of a character (physicsShapeOffset) : on the blocks, it does not touch the walls on their edges ;
	// its feet (the bottom of the shape : its drawing order, ska GraphicSystem) are never inside a wall under it
	ska::Rectangle PhysicsBox(const ska::HitboxComponent& hitbox, std::optional<ska::Point<int>> shapeSize) {
		constexpr auto blockSize = 48;
		const auto size = shapeSize.value_or(ska::Point<int>{ static_cast<int>(hitbox.width), static_cast<int>(hitbox.height) });
		const auto offset = CustomEntityManager::physicsShapeOffset(size, blockSize);
		return { offset.x, offset.y, size.x, size.y };
	}
}

void CustomEntityManager::fillCharacter(CustomEntityManager& em, ska::cp::Space& space, ska::EntityId character, float friction, float rotationFriction) {
	fill(em, space, character, friction, rotationFriction, {});
}

void CustomEntityManager::fillTrainer(CustomEntityManager& em, ska::cp::Space& space, ska::EntityId trainer) {
	fill(em, space, trainer, DEFAULT_FRICTION, DEFAULT_ROTATION_FRICTION, ska::Point<int>{ TrainerShape, TrainerShape });
}

void CustomEntityManager::fill(CustomEntityManager& em, ska::cp::Space& space, ska::EntityId character, float friction, float rotationFriction, std::optional<ska::Point<int>> shapeSize) {
	const auto& hasBeenCreated = em.hasComponent<ska::cp::HitboxComponent>(character);
	const auto& point = em.getComponent<ska::PositionComponent>(character);
	
	auto& hitbox = em.getComponent<ska::HitboxComponent>(character);
	const auto& fc = em.getComponent<ska::ForceComponent>(character);
	
	if (hasBeenCreated) {
		hitbox.yOffset += hitbox.height;
	}

	auto bc = ska::cp::BuildControlledRectangleHitbox(space, point, PhysicsBox(hitbox, shapeSize), fc.weight, character);
	ska::cp::AddTopDownConstraints(space, bc, friction * 50, rotationFriction * 30);
	
	hitbox.yOffset -= hitbox.height;

	em.addComponent(character, std::move(bc));
}

ska::EntityId CustomEntityManager::createMovingCharacterNG(CustomEntityManager& em, ska::cp::Space& space, const ska::Point<int> startBlockPos, const int id, const unsigned int worldBlockSize, const std::string& name) {
	const auto character = em.createCharacter(startBlockPos, id, worldBlockSize, name);
	fillCharacter(em, space, character);
	return character;
}

ska::EntityId CustomEntityManager::createTrainerNG(CustomEntityManager & em, ska::cp::Space& space, const ska::Point<int> startBlockPos, const unsigned int worldBlockSize, const std::string& name) {
	const auto trainer = em.createTrainer(startBlockPos, worldBlockSize);
	fillTrainer(em, space, trainer);
	
	return trainer;
}

ska::Point<int> CustomEntityManager::physicsShapeOffset(const ska::Point<int> shapeSize, const int blockSize) {
	return { (blockSize - shapeSize.x) / 2, blockSize - shapeSize.y - 1 };
}

void CustomEntityManager::place(ska::EntityManager& em, ska::cp::Space& space, ska::EntityId entity, ska::Point<float> position) {
	if (em.hasComponent<ska::PositionComponent>(entity)) {
		auto& pc = em.getComponent<ska::PositionComponent>(entity);
		pc.x = static_cast<long>(position.x);
		pc.y = static_cast<long>(position.y);
	}
	if (em.hasComponent<ska::cp::HitboxComponent>(entity)) {
		const auto& hitbox = em.getComponent<ska::cp::HitboxComponent>(entity);
		space.getBody(hitbox.bodyIndex).setPosition(ska::cp::Vect{ position.x, position.y });
		if (hitbox.controlBodyIndex != hitbox.bodyIndex) {
			space.getBody(hitbox.controlBodyIndex).setPosition(ska::cp::Vect{ position.x, position.y });
		}
	}
}

ska::EntityId CustomEntityManager::createCharacterNG(CustomEntityManager& em, ska::cp::Space& space, const ska::Point<int> startBlockPos, const int id, const unsigned int worldBlockSize, const std::string& name) {
	const auto character = em.createCharacter(startBlockPos, id, worldBlockSize, name);
	const auto& point = em.getComponent<ska::PositionComponent>(character);
	auto& hitbox = em.getComponent<ska::HitboxComponent>(character);
	auto& fc = em.getComponent<ska::ForceComponent>(character);

	auto bc = ska::cp::BuildRectangleHitbox(space, point, PhysicsBox(hitbox, {}), fc.weight, character);
	ska::cp::AddTopDownConstraints(space, bc, DEFAULT_FRICTION * 50, DEFAULT_ROTATION_FRICTION * 30);

	hitbox.yOffset -= hitbox.height;

	em.addComponent(character, std::move(bc));
	return character;
}
