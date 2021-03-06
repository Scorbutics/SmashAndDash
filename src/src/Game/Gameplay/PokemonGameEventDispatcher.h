#pragma once
#include "Core/Data/Events/ExtensibleGameEventDispatcher.h"
#include "./Fight/StatisticsChangeEvent.h"
#include "./Fight/BattleEvent.h"
#include "./Data/EntityLoadEvent.h"
#include "./Data/SettingsChangeEvent.h"
#include "./Data/DialogEvent.h"
#include "Data/MapEvent.h"


using PokemonGameEventDispatcher = ska::ExtensibleGameEventDispatcher<BattleEvent, StatisticsChangeEvent, SettingsChangeEvent, EntityLoadEvent, MapEvent, DialogEvent>;

