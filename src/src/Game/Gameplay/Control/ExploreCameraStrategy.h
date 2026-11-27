#pragma once
#include "Graphic/System/CameraStrategy.h"
#include "Core/ECS/EntityManager.h"
#include "Core/Point.h"

// The camera of the map : it follows the player (CameraFocusedComponent), a little ahead of where it walks
// (GridMovementComponent::velocity : LeadTime of its walk, Lead pixels at most), the lead moving smoothly (LeadSmoothing ms)
class ExploreCameraStrategy : public ska::CameraStrategy {
public:
	explicit ExploreCameraStrategy(ska::EntityManager& entityManager);
	~ExploreCameraStrategy() override = default;

	static constexpr float LeadTime = 0.22F;
	static constexpr float Lead = 56.F;
	static constexpr float LeadSmoothing = 320.F;

	std::pair<ska::Rectangle, const ska::EntityId*> focus(const std::unordered_set<ska::EntityId>& processed) override;

private:
	ska::EntityManager& m_entityManager;
	ska::Point<float> m_lead;
	unsigned int m_last = 0;
};
