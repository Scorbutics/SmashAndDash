#pragma once
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Graphic/System/CameraStrategy.h"
#include "../BattleComponent.h"
#include "../FighterComponent.h"
#include "../BattleStage.h"

namespace ska {
	class CameraSystem;
}

// The camera of a battle follows a point (BattleCameraSystem)
class BattleCameraStrategy : public ska::CameraStrategy {
public:
	std::pair<ska::Rectangle, ska::EntityId*> focus(const std::unordered_set<ska::EntityId>&) override {
		return std::make_pair(ska::Rectangle{ static_cast<int>(point.x), static_cast<int>(point.y), 0, 0 }, nullptr);
	}
	ska::Point<float> point;
};

// The camera of a battle, as in the anime : it zooms in when the fight starts, follows the Pokemon of the player a little
// more than its opponents while it shows them all, drifts slowly, pushes in on the hits, looks at a Pokemon preparing a
// long attack, closes in on a K.O.
class BattleCameraSystem : public ska::System<ska::RequiredComponent<ska::PositionComponent, ska::HitboxComponent, BattleComponent, FighterComponent>, ska::PossibleComponent<>> {
public:
	BattleCameraSystem(ska::EntityManager& em, ska::CameraSystem& camera, BattleStage& stage);
	BattleCameraSystem(const BattleCameraSystem&) = delete;
	BattleCameraSystem& operator=(const BattleCameraSystem&) = delete;
	~BattleCameraSystem() override = default;

	static constexpr float MinZoom = 0.85F;
	static constexpr float MaxZoom = 1.7F;

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	ska::Point<float> center(ska::EntityId entity);

	ska::CameraSystem& m_camera;
	BattleStage& m_stage;
	BattleCameraStrategy* m_strategy = nullptr;
	ska::Point<float> m_focus;
	float m_zoom = 1.F;
	// A push in (a hit), slowing down
	float m_kick = 0.F;
	bool m_wasFrozen = false;
	unsigned int m_time = 0;
};
