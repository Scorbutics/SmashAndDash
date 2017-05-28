#include "PrefabEntityManager.h"

#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Graphic/CameraFocusedComponent.h"
#include "ECS/Basics/Physic/GravityAffectedComponent.h"
#include "ECS/Basics/Script/ScriptAwareComponent.h"
#include "Graphic/GraphicComponent.h"
#include "ECS/Basics/Graphic/DirectionalAnimationComponent.h"
#include "ECS/Basics/Input/InputComponent.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "ECS/Basics/Physic/HitboxComponent.h"
#include "Graphic/HasShadowComponent.h"
#include "ECS/Basics/Physic/CollidableComponent.h"
#include "Utils/SpritePath.h"

ska::PrefabEntityManager::PrefabEntityManager() {
}

ska::EntityId ska::PrefabEntityManager::createCharacter(const Point<int> startPos, const int id, const unsigned int worldBlockSize) {
	EntityId hero = createEntity();
	PositionComponent pc;
	pc.x = startPos.x * worldBlockSize;
	pc.y = startPos.y * worldBlockSize;
	pc.z = 0;
	addComponent<PositionComponent>(hero, pc);
	MovementComponent mc;
	ForceComponent fc;
	fc.weight = 65.0;
	GravityAffectedComponent gac;
	gac.friction = 20;
	addComponent<GravityAffectedComponent>(hero, gac);
	addComponent<ForceComponent>(hero, fc);
	addComponent<MovementComponent>(hero, mc);
	GraphicComponent gc;
	gc.sprite.resize(1);
	gc.sprite[0].load(SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, id), 6, 8, 3);
	gc.sprite[0].setDelay(100);

	HitboxComponent hc;
	hc.xOffset = 20;
	hc.yOffset = gc.sprite[0].getHeight() * 3 / 5;
	hc.height = gc.sprite[0].getHeight() - hc.yOffset;
	hc.width = gc.sprite[0].getWidth() - 2 * hc.xOffset;
	addComponent<HitboxComponent>(hero, hc);

	addComponent<CollidableComponent>(hero, CollidableComponent());
	addComponent<HasShadowComponent>(hero, HasShadowComponent());
	addComponent<GraphicComponent>(hero, gc);

	addComponent<DirectionalAnimationComponent>(hero, DirectionalAnimationComponent());

	return hero;
}

ska::EntityId ska::PrefabEntityManager::createTrainer(const Point<int> startPos, const unsigned int worldBlockSize) {
	EntityId hero = createCharacter(startPos, 0, worldBlockSize);
	addComponent<CameraFocusedComponent>(hero, CameraFocusedComponent());
	InputComponent ic;
	ic.movePower = 200;
	ic.jumpPower = 1200;
	addComponent<InputComponent>(hero, ic);
	ScriptAwareComponent sac;
	addComponent<ScriptAwareComponent>(hero, sac);
	return hero;
}

ska::PrefabEntityManager::~PrefabEntityManager() {
}
