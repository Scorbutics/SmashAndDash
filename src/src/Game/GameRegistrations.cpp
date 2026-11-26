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
#include "Gameplay/Fight/FighterComponent.h"
#include "Gameplay/Control/MovementModeComponent.h"
#include "Gameplay/Control/GridMovementComponent.h"
#include "Gameplay/PokeballComponent.h"
#include "Gameplay/Mobs/MobSpawnAreaComponent.h"
#include "Gameplay/Mobs/WildPokemonComponent.h"
#include "Gameplay/Mobs/ArrivalComponent.h"
#include "Graphic/GUI/SlotPokemon.h"

SKA_DEFINE_COMPONENT(SkillsHolderComponent);
SKA_DEFINE_COMPONENT(SkillComponent);
SKA_DEFINE_COMPONENT(FightComponent);
SKA_DEFINE_COMPONENT(BattleComponent);
SKA_DEFINE_COMPONENT(FighterComponent);
SKA_DEFINE_COMPONENT(MovementModeComponent);
SKA_DEFINE_COMPONENT(GridMovementComponent);
SKA_DEFINE_COMPONENT(PokeballComponent);
SKA_DEFINE_COMPONENT(MobSpawnAreaComponent);
SKA_DEFINE_COMPONENT(WildPokemonComponent);
SKA_DEFINE_COMPONENT(ArrivalComponent);

namespace ska {
	SKA_DEFINE_GUI_EVENT(ska::ValueChangedEventListener<int>);
	SKA_DEFINE_GUI_EVENT(ska::ValueChangedEventListener<SlotPokemonDataPtr*>);
}
