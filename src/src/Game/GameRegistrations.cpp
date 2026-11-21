// Type registrations required by ska for the game types : ECS components and GUI events
// (their identifiers are defined once, in the game library).
#include "Game/__internalConfig/LoggerConfig.h"
#include "Core/ECS/Component.h"
#include "GUI/Utils/WidgetMaskHelper.h"
#include "GUI/Events/ValueChangedEvent.h"
#include "Gameplay/Fight/SkillsHolderComponent.h"
#include "Gameplay/Fight/SkillComponent.h"
#include "Gameplay/Fight/FightComponent.h"
#include "Gameplay/Fight/BattleComponent.h"
#include "Gameplay/PokeballComponent.h"
#include "Gameplay/Mobs/MobSpawnAreaComponent.h"
#include "Graphic/GUI/SlotPokemon.h"

SKA_DEFINE_COMPONENT(SkillsHolderComponent);
SKA_DEFINE_COMPONENT(SkillComponent);
SKA_DEFINE_COMPONENT(FightComponent);
SKA_DEFINE_COMPONENT(BattleComponent);
SKA_DEFINE_COMPONENT(PokeballComponent);
SKA_DEFINE_COMPONENT(MobSpawnAreaComponent);

namespace ska {
	SKA_DEFINE_GUI_EVENT(ska::ValueChangedEventListener<int>);
	SKA_DEFINE_GUI_EVENT(ska::ValueChangedEventListener<SlotPokemonDataPtr*>);
}
