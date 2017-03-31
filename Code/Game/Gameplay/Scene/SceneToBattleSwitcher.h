#pragma once
#include "Point.h"
#include "Core/Scene/SceneBase.h"
#include "../World/WorldScene.h"
#include "../Fight/FightComponent.h"
#include "SceneFight.h"

class SceneToBattleSwitcher {
public:
	SceneToBattleSwitcher(FightComponent& fc, WorldScene& ws) :
		m_ws(ws), m_fightComponent(fc) {}
	SceneToBattleSwitcher& operator=(const SceneToBattleSwitcher&) = delete;
	~SceneToBattleSwitcher() = default;

	template<class EM, class ED>
	void switchTo(ska::SceneBase<EM, ED>& lastScene) const {
		lastScene.template makeNextScene<SceneFight>(m_ws, m_fightPos, m_fightComponent);
	}

protected:
	WorldScene& m_ws;
	const FightComponent& m_fightComponent;
	ska::Point<int> m_fightPos;
};

