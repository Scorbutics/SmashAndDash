#include "CustomEntityManager.h"
#include "./Fight/FightComponent.h"
#include "./Mobs/MobSpawnAreaComponent.h"
#include "../ska/Graphic/GraphicComponent.h"
#include "./Fight/SkillComponent.h"
#include "../ska/Physic/MovementComponent.h"
#include "../ska/Physic/PositionComponent.h"
#include "../ska/Physic/HitboxComponent.h"
#include "../ska/Physic/ForceComponent.h"
#include "../ska/Physic/CollidableComponent.h"
#include "../ska/Inputs/Readers/IniReader.h"
#include "../ska/Graphic/SpritePath.h"
#include "./Fight/SkillsHolderComponent.h"
#include "../ska/Utils/NumberUtils.h"
#include "../Utils/IDs.h"

CustomEntityManager::CustomEntityManager()
{
}

ska::EntityId CustomEntityManager::createSkill(const SkillsHolderComponent& shc, unsigned int index) {

	SkillComponent sc;
	//sc.noise = reader.getInt("Particle slope_noise");
	const SkillDescriptor& sd = shc.skills[index];
	sc.speed = sd.speed;
	sc.damage = -sd.buffEnemy.hp;
	sc.knockback = sd.knockback;
	sc.range = sd.range;
	const int maxNoise = sd.noise;
	sc.noise = ska::NumberUtils::random(maxNoise/2, maxNoise);
	sc.amplitude = sd.amplitude;
	

	ska::EntityId skill = createEntity();

	ska::PositionComponent pcSkill;
	ska::GraphicComponent gcSkill;
	gcSkill.desiredPriority = SKILLS_DEFAULT_DISPLAY_PRIORITY;
	gcSkill.sprite.resize(1);
	gcSkill.sprite[0].load(ska::SpritePath::getInstance().getPath(SPRITEBANK_SKILL, sd.id), 2, 2, 2);
	addComponent<ska::MovementComponent>(skill, ska::MovementComponent());
	addComponent<SkillComponent>(skill, sc);
	addComponent<ska::PositionComponent>(skill, pcSkill);
	ska::HitboxComponent hc;
	hc.xOffset = gcSkill.sprite[0].getWidth() / 2 - 1;
	hc.yOffset = gcSkill.sprite[0].getHeight() / 2 - 1;
	hc.width = gcSkill.sprite[0].getWidth() / 2;
	hc.height = gcSkill.sprite[0].getHeight() / 2;
	addComponent<ska::ForceComponent>(skill, ska::ForceComponent());
	addComponent<ska::HitboxComponent>(skill, hc);
	addComponent<ska::GraphicComponent>(skill, gcSkill);
	addComponent<ska::CollidableComponent>(skill, ska::CollidableComponent());
	return skill;
}

ska::EntityId CustomEntityManager::createTrainer(const ska::Point<int> startBlockPos, const unsigned int worldBlockSize) {
	ska::EntityId trainer = PrefabEntityManager::createTrainer(startBlockPos, worldBlockSize);
	addComponent<MobSpawnAreaComponent>(trainer, MobSpawnAreaComponent());
	addComponent<FightComponent>(trainer, FightComponent());
	return trainer;
}

ska::EntityId CustomEntityManager::createCharacter(const ska::Point<int> startBlockPos, const int id, const unsigned int worldBlockSize) {
	return PrefabEntityManager::createCharacter(startBlockPos, id, worldBlockSize);
}

CustomEntityManager::~CustomEntityManager()
{
}
