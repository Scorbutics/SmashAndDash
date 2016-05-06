#include "CustomEntityManager.h"
#include "./Fight/FightComponent.h"
#include "./Mobs/MobSpawnAreaComponent.h"
#include "../ska/Graphic/GraphicComponent.h"
#include "./Fight/SkillComponent.h"
#include "../ska/Physic/MovementComponent.h"
#include "../ska/Physic/PositionComponent.h"
#include "../ska/Physic/HitboxComponent.h"
#include "../ska/Physic/ForceComponent.h"
#include "../ska/Inputs/Readers/IniReader.h"
#include "../ska/Graphic/SpritePath.h"
#include "./Fight/SkillsHolderComponent.h"
#include "../ska/Utils/NumberUtils.h"

CustomEntityManager::CustomEntityManager()
{
}

ska::EntityId CustomEntityManager::createSkill(const ska::IniReader& reader, const SkillsHolderComponent& shc, unsigned int index) {

	SkillComponent sc;
	//sc.noise = reader.getInt("Particle slope_noise");
	sc.speed = (float) reader.getInt("Particle speed");
	sc.damage = reader.getInt("Particle damage");
	sc.knockback = reader.getInt("Particle knockback");
	const int maxNoise = reader.getInt("Particle noise");
	sc.noise = ska::NumberUtils::random(maxNoise/2, maxNoise);

	const SkillDescriptor& sd = shc.skills[index];

	ska::EntityId skill = createEntity();

	ska::PositionComponent pcSkill;
	ska::GraphicComponent gcSkill;
	gcSkill.sprite.resize(1);
	gcSkill.sprite[0].load(ska::SpritePath::getInstance().getPath(SPRITEBANK_SKILL, sd.id), 2, 2, 2);
	addComponent<ska::MovementComponent>(skill, ska::MovementComponent());
	addComponent<SkillComponent>(skill, sc);
	addComponent<ska::PositionComponent>(skill, pcSkill);
	ska::HitboxComponent hc;
	hc.xOffset = 0;
	hc.yOffset = 0;
	hc.width = gcSkill.sprite[0].getWidth();
	hc.height = gcSkill.sprite[0].getHeight();

	addComponent<ska::HitboxComponent>(skill, hc);
	addComponent<ska::GraphicComponent>(skill, gcSkill);

	return skill;
}

ska::EntityId CustomEntityManager::createTrainer(const ska::Point<int> startBlockPos, const unsigned int worldBlockSize) {
	ska::EntityId trainer = ska::PrefabEntityManager::createTrainer(startBlockPos, worldBlockSize);
	addComponent<MobSpawnAreaComponent>(trainer, MobSpawnAreaComponent());
	addComponent<FightComponent>(trainer, FightComponent());
	return trainer;
}

ska::EntityId CustomEntityManager::createCharacter(const ska::Point<int> startBlockPos, const int id, const unsigned int worldBlockSize) {
	return ska::PrefabEntityManager::createCharacter(startBlockPos, id, worldBlockSize);
}

CustomEntityManager::~CustomEntityManager()
{
}
