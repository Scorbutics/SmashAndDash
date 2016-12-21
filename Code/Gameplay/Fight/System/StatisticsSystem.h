#pragma once
#include <unordered_set>
#include "../../../ska/ECS/System.h"
#include "../../../ska/Physic/PositionComponent.h"
#include "../../../ska/Scene/SceneHolder.h"
#include "../BattleComponent.h"
#include "../SkillsHolderComponent.h"
#include "../../World/WorldScene.h"

namespace ska {
	class Window;
}

class StatisticsSystem : public ska::System<std::unordered_set<ska::EntityId>, ska::PositionComponent, BattleComponent> {
public:
	StatisticsSystem(ska::Window& w, ska::EntityManager& em, ska::SceneHolder& sceneHolder, ska::InputContextManager& icm, WorldScene& ws, ska::GameEventDispatcher& ged);
	virtual ~StatisticsSystem();

protected:
	virtual void refresh() override;

private:
	ska::SceneHolder& m_sceneHolder;
	ska::InputContextManager& m_playerICM;
	WorldScene& m_worldScene;
	ska::Window& m_window;
	ska::GameEventDispatcher& m_ged;
};