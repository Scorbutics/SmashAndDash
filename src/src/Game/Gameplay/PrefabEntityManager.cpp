#include "Game/__internalConfig/LoggerConfig.h"
#include "PrefabEntityManager.h"

#include "Core/ECS/Basics/Physic/MovementComponent.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Graphic/CameraFocusedComponent.h"
#include "Core/ECS/Basics/Script/ScriptAwareComponent.h"
#include "Graphic/GraphicComponent.h"
#include "Core/ECS/Basics/Input/InputComponent.h"
#include "Core/ECS/Basics/Physic/ForceComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Graphic/HasShadowComponent.h"
#include "Core/ECS/Basics/Physic/CollidableComponent.h"
#include "Core/Utils/SpritePath.h"
#include "Core/Utils/SkaConstants.h"

ska::PrefabEntityManager::PrefabEntityManager(GameEventDispatcher& ged) :
	EntityManager(ged) {
}

ska::EntityId ska::PrefabEntityManager::createCharacter(const Point<int> startPos, const int spriteId, const unsigned int worldBlockSize, const std::string& name) {
	auto hero = createEntity();
	if (!name.empty()) {
		// Found by name by the scripts (EntityLocator)
		setName(hero, name);
	}
	PositionComponent pc;
	pc.x = startPos.x * worldBlockSize;
	pc.y = startPos.y * worldBlockSize;
	pc.z = 0;
	pc.layer = 1;
	addComponent<PositionComponent>(hero, std::move(pc));
	MovementComponent mc;
	ForceComponent fc;
	fc.weight = 65.0;
	fc.bounciness = 10.F;
	fc.maxSpeed = 240.F;
	addComponent<ForceComponent>(hero, std::move(fc));
	addComponent<MovementComponent>(hero, std::move(mc));
	GraphicComponent gc;
	gc.animatedSprites.resize(1);
	gc.animatedSprites[0].load(SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, spriteId), 6, 8, 3);
	gc.animatedSprites[0].setDelay(100);

	HitboxComponent hc;
	hc.xOffset = 20;
	hc.yOffset = gc.animatedSprites[0].getHeight() * 0.66;
	hc.height = gc.animatedSprites[0].getHeight() - hc.yOffset;
	hc.width = gc.animatedSprites[0].getWidth() - 2 * hc.xOffset;
	// The drawing on its block, as its physics shape (CustomEntityManager::physicsShapeOffset) : centred, the bottom of the
	// frame (its feet) on the bottom of the block. The hitbox alone placed it 6 px right and 6 px low (a 72 px frame)
	const auto spriteWidth = static_cast<int>(gc.animatedSprites[0].getWidth());
	const auto spriteHeight = static_cast<int>(gc.animatedSprites[0].getHeight());
	const auto block = static_cast<int>(worldBlockSize);
	const auto placed = gc.spriteOrigin(0, 0, hc);
	gc.drawOffset = { (block - spriteWidth) / 2 - placed.x, block + 1 - spriteHeight - placed.y };
	// It walks block by block : the depth of the upper of its two blocks during a step (the leaves of the tall grass
	// cover it during all of its step). A fighter moves freely : StateFight sorts it by its position
	gc.sortStep = block;
	addComponent<HitboxComponent>(hero, std::move(hc));

	addComponent<CollidableComponent>(hero, CollidableComponent());
	//TODO suppr et mettre un second sprite representant l'ombre a la place
	addComponent<HasShadowComponent>(hero, HasShadowComponent());
	addComponent<GraphicComponent>(hero, std::move(gc));

	addComponent<AnimationComponent>(hero, AnimationComponent());

	return hero;
}

ska::EntityId ska::PrefabEntityManager::createTrainer(const Point<int> startPos, const unsigned int worldBlockSize) {
	const auto hero = createCharacter(startPos, 0, worldBlockSize, "trainer");
	// It walks freely (GridMovementSystem) : sorted by its exact position
	getComponent<GraphicComponent>(hero).sortStep = 0;
	addComponent<CameraFocusedComponent>(hero, CameraFocusedComponent());
	InputComponent ic;
	ic.movePower = 25;
	// A small hop on the map (the gravity and the friction of the height : SpaceSystem, ZGravityForceSystem)
	ic.jumpPower = 8;
	addComponent<InputComponent>(hero, std::move(ic));
	ScriptAwareComponent sac;
	addComponent<ScriptAwareComponent>(hero, std::move(sac));
	return hero;
}
