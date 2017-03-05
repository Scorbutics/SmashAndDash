#pragma once

#include "../ska/Core/GameEventDispatcher.h"
#include "./Fight/StatisticsChangeEvent.h"
#include "./Fight/BattleEvent.h"
#include "./Data/EntityLoadEvent.h"
#include "./Data/SettingsChangeEvent.h"

template <class ...ET>
class SEventDispatcher : 
	public ska::GameEventDispatcher,
	public ska::Observable<ET>... {
public:
	SEventDispatcher() = default;
	SEventDispatcher(const SEventDispatcher&) = delete;
	virtual ~SEventDispatcher() = default;

private:

};
using PokemonGameEventDispatcher = SEventDispatcher<BattleEvent, StatisticsChangeEvent, SettingsChangeEvent, EntityLoadEvent>;