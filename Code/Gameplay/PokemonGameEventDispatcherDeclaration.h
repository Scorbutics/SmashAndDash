#pragma once
#include "../Gameplay./PokemonGameEventDispatcher.h"
#include "./Fight/StatisticsChangeEvent.h"
#include "./Fight/BattleEvent.h"
#include "./Data/EntityLoadEvent.h"
#include "./Data/SettingsChangeEvent.h"

using PokemonGameEventDispatcher = SEventDispatcher<BattleEvent, StatisticsChangeEvent, SettingsChangeEvent, EntityLoadEvent>;
