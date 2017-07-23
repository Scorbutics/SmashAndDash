#pragma once
#include "Point.h"
#include "Core/State/StateBase.h"
#include "../World/WorldState.h"
#include "../Fight/FightComponent.h"
#include "StateFight.h"

class StateToBattleSwitcher {
public:
	StateToBattleSwitcher(FightComponent& fc, ska::Point<int> fightPos, WorldState& ws, ska::Point<int> screenSize) :
		m_ws(ws), 
		m_fightComponent(fc), 
		m_fightPos(fightPos),
		m_screenSize(screenSize) {}
	StateToBattleSwitcher& operator=(const StateToBattleSwitcher&) = delete;
	~StateToBattleSwitcher() = default;

	template<class EM, class ED>
	void switchTo(ska::StateBase<EM, ED>& lastState) const {
		lastState.template makeNextStateAndTransmitLinkedSubstates<ska::StateBase<EM, ED>, StateFight>(lastState, m_ws, m_fightPos, m_fightComponent, m_screenSize);
	}

protected:
	WorldState& m_ws;
	const FightComponent& m_fightComponent;
	ska::Point<int> m_fightPos;
	ska::Point<int> m_screenSize;
};

