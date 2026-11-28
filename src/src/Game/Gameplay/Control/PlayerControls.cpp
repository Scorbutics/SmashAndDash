#include "Game/__internalConfig/LoggerConfig.h"
#include "PlayerControls.h"
#include "Core/ECS/EntityManager.h"
#include "Core/ECS/Basics/Input/InputComponent.h"
#include "Core/ECS/Basics/Script/ScriptAwareComponent.h"

void HoldPlayerControls(ska::EntityManager& entities, ska::EntityId character) {
	entities.hold<ska::InputComponent>(character);
	entities.hold<ska::ScriptAwareComponent>(character);
}

void ReleasePlayerControls(ska::EntityManager& entities, ska::EntityId character) {
	entities.release<ska::ScriptAwareComponent>(character);
	entities.release<ska::InputComponent>(character);
}

bool HasPlayerControls(ska::EntityManager& entities, ska::EntityId character) {
	return entities.hasComponent<ska::InputComponent>(character);
}
