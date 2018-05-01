#include "CustomEntityManager.h"
#include "./Fight/FightComponent.h"
#include "./Mobs/MobSpawnAreaComponent.h"
#include "Graphic/GraphicComponent.h"
#include "./Fight/SkillComponent.h"
#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Physic/HitboxComponent.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "ECS/Basics/Physic/CollidableComponent.h"
#include "Utils/SpritePath.h"
#include "./Fight/SkillsHolderComponent.h"
#include "Utils/NumberUtils.h"
#include "../Utils/IDs.h"

#include "Physic/BuildHitbox.h"

CustomEntityManager::CustomEntityManager(ska::GameEventDispatcher& ged) : 
	ska::PrefabEntityManager(ged) {
}

ska::EntityId CustomEntityManager::createSkill(const SkillsHolderComponent& shc, unsigned int index) {

	SkillComponent sc;
	//sc.noise = reader.getInt("Particle slope_noise");
	const auto& sd = shc.skills[index];
	sc.speed = sd.speed;
	sc.damage = -sd.buffEnemy.hp;
	sc.knockback = sd.knockback;
	sc.range = sd.range;
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

ska::EntityId CustomEntityManager::createCharacter(const ska::Point<int> startBlockPos, const int id, const unsigned int worldBlockSize) {
	return PrefabEntityManager::createCharacter(startBlockPos, id, worldBlockSize);
}

ska::EntityId CustomEntityManager::createTrainerNG(CustomEntityManager & em, ska::cp::Space& space, const ska::Point<int> startBlockPos, const unsigned int worldBlockSize) {
	auto trainer = em.createTrainer(startBlockPos, worldBlockSize);
	const auto& point = em.getComponent<ska::PositionComponent>(trainer);
	const auto& hitbox = em.getComponent<ska::HitboxComponent>(trainer);
	auto bc = ska::cp::BuildRectangleHitbox(space, { static_cast<int>(point.x), static_cast<int>(point.y), static_cast<int>(hitbox.width), static_cast<int>(hitbox.height) }, 200.f, 500.f, trainer);
	em.addComponent(trainer, std::move(bc));
	return trainer;
}

ska::EntityId CustomEntityManager::createCharacterNG(CustomEntityManager& em, ska::cp::Space& space, const ska::Point<int> startBlockPos, const int id, const unsigned int worldBlockSize) {
	auto character = em.createCharacter(startBlockPos, id, worldBlockSize);
	const auto& point = em.getComponent<ska::PositionComponent>(character);
	const auto& hitbox = em.getComponent<ska::HitboxComponent>(character);
	auto bc = ska::cp::BuildRectangleHitbox(space, { static_cast<int>(point.x), static_cast<int>(point.y), static_cast<int>(hitbox.width), static_cast<int>(hitbox.height) }, 200.f, 500.f, character);
	em.addComponent(character, std::move(bc));
	return character;
}
