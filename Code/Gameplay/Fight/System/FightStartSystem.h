#pragma once
#include <unordered_set>
#include "../../../ska/Physic/PositionComponent.h"
#include "../FightComponent.h"
#include "../../../ska/ECS/System.h"
#include "../../../ska/Scene/SceneHolder.h"
#include "../../../ska/Inputs/InputContextManager.h"
#include "../../../ska/Graphic/GraphicComponent.h"

class WorldScene;

class FightStartSystem : public ska::System<std::unordered_set<ska::EntityId>, ska::PositionComponent, FightComponent, ska::GraphicComponent> {
public:
	FightStartSystem(ska::SceneHolder& sceneHolder, WorldScene& ws, ska::InputContextManager& icm, const ska::EntityId player);
	virtual ~FightStartSystem();
	virtual void refresh() override;
private:
	WorldScene& m_worldScene;
	ska::InputContextManager& m_icm;
	ska::SceneHolder& m_sceneHolder;
	const ska::EntityId m_player;
	unsigned int m_t0;
};


