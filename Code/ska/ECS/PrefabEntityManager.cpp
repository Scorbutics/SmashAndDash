#include "PrefabEntityManager.h"
#include "../Physic/MovementComponent.h"
#include "../Physic/PositionComponent.h"
#include "../Graphic/CameraFocusedComponent.h"
#include "../Physic/GravityAffectedComponent.h"
#include "../Script/ScriptAwareComponent.h"
#include "../Graphic/GraphicComponent.h"
#include "../Graphic/DirectionalAnimationComponent.h"
#include "../Inputs/InputComponent.h"
#include "../Physic/ForceComponent.h"
#include "../Physic/HitboxComponent.h"
#include "../Graphic/SpritePath.h"
#include "../Graphic/HasShadowComponent.h"

ska::PrefabEntityManager::PrefabEntityManager() {
}

ska::EntityId ska::PrefabEntityManager::createCharacter(const ska::Point<int> startPos, const int id, const unsigned int worldBlockSize) {
	ska::EntityId hero = createEntity();
	ska::PositionComponent pc;
	pc.x = startPos.x*worldBlockSize;
	pc.y = startPos.y*worldBlockSize;
	pc.z = 0;
	addComponent<ska::PositionComponent>(hero, pc);
	ska::MovementComponent mc;
	memset(&mc, 0, sizeof(ska::MovementComponent));
	ska::ForceComponent fc;
	memset(&fc, 0, sizeof(fc));
	fc.weight = 65.0;
	ska::GravityAffectedComponent gac;
	gac.friction = 20;
	addComponent<ska::GravityAffectedComponent>(hero, gac);
	addComponent<ska::ForceComponent>(hero, fc);
	addComponent<ska::MovementComponent>(hero, mc);
	ska::GraphicComponent gc;
	gc.sprite.load(ska::SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, id), 6, 8, 3);
	gc.sprite.setDelay(100);
	addComponent<ska::GraphicComponent>(hero, gc);
	addComponent<ska::HasShadowComponent>(hero, ska::HasShadowComponent());
	addComponent<ska::DirectionalAnimationComponent>(hero, ska::DirectionalAnimationComponent());
	ska::HitboxComponent hc;
	hc.xOffset = 20;
	hc.yOffset = gc.sprite.getHeight() *  3 / 5;
	hc.height = gc.sprite.getHeight() - hc.yOffset;
	hc.width = gc.sprite.getWidth() - 2 * hc.xOffset;
	addComponent<ska::HitboxComponent>(hero, hc);
	return hero;
}

ska::EntityId ska::PrefabEntityManager::createTrainer(const ska::Point<int> startPos, const unsigned int worldBlockSize) {
	ska::EntityId hero = createCharacter(startPos, 0, worldBlockSize);
	addComponent<ska::CameraFocusedComponent>(hero, ska::CameraFocusedComponent());
	ska::InputComponent ic;
	ic.movePower = 100;
	ic.jumpPower = 130 << 3;
	addComponent<ska::InputComponent>(hero, ic);
	ska::ScriptAwareComponent sac;
	addComponent<ska::ScriptAwareComponent>(hero, sac);
	return hero;
}

ska::PrefabEntityManager::~PrefabEntityManager() {
}