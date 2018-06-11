#include "PrefabEntityManager.h"

#include "ECS/Basics/Physic/MovementComponent.h"
#include "ECS/Basics/Physic/PositionComponent.h"
#include "ECS/Basics/Graphic/CameraFocusedComponent.h"
#include "ECS/Basics/Physic/GravityAffectedComponent.h"
#include "ECS/Basics/Script/ScriptAwareComponent.h"
#include "Graphic/GraphicComponent.h"
#include "ECS/Basics/Input/InputComponent.h"
#include "ECS/Basics/Physic/ForceComponent.h"
#include "ECS/Basics/Physic/HitboxComponent.h"
#include "Graphic/HasShadowComponent.h"
#include "ECS/Basics/Physic/CollidableComponent.h"
#include "Utils/SpritePath.h"
#include "Utils/SkaConstants.h"

ska::PrefabEntityManager::PrefabEntityManager(GameEventDispatcher& ged) :
	EntityManager(ged) {
}

ska::EntityId ska::PrefabEntityManager::createCharacter(const Point<int> startPos, const int spriteId, const unsigned int worldBlockSize, const std::string& name) {
	auto hero = createEntity(name);
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
	GravityAffectedComponent gac;
	gac.friction = 60.F;
	gac.rotationFriction = 10.F;
	addComponent<GravityAffectedComponent>(hero, std::move(gac));
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
	addComponent<CameraFocusedComponent>(hero, CameraFocusedComponent());
	InputComponent ic;
	ic.movePower = 25;
	ic.jumpPower = 80;
	addComponent<InputComponent>(hero, std::move(ic));
	ScriptAwareComponent sac;
	addComponent<ScriptAwareComponent>(hero, std::move(sac));
	return hero;
}
