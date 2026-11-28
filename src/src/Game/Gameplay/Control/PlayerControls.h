#pragma once
#include "Core/ECS/ECSDefines.h"

namespace ska {
	class EntityManager;
}

// The controls of the player on its character : its input (InputComponent : it walks, it jumps) and its scripts
// (ScriptAwareComponent : the action key talks, opens ; the blocks it enters). Held (EntityManager::hold, counted) by what
// takes the keys a while : the menu of the trainer, a dialog, a battle ; back when every one released them
void HoldPlayerControls(ska::EntityManager& entities, ska::EntityId character);
void ReleasePlayerControls(ska::EntityManager& entities, ska::EntityId character);
// Its input is there (not held) : the keys move it
bool HasPlayerControls(ska::EntityManager& entities, ska::EntityId character);
