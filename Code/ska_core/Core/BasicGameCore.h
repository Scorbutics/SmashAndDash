#pragma once
#include "GameCore.h"
#include "../../Game/Gameplay/PrefabEntityManager.h"
#include "../Data/Events/GameEventDispatcher.h"
#include "../Draw/VectorDrawableContainer.h"
#include "../../ska_audio/Audio/SoundRenderer.h"

namespace ska {
	using BasicGameCore = GameCore<PrefabEntityManager, GameEventDispatcher, VectorDrawableContainer, SoundRenderer>;
}
