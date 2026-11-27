#pragma once
#include <unordered_map>
#include <vector>
#include "Core/ECS/System.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Graphic/AnimationComponent.h"
#include "Graphic/GraphicComponent.h"
#include "../../Graphic/GUI/Canvas.h"
#include "GridMovementComponent.h"

namespace ska {
	class CameraSystem;
	class TileWorld;
	class DrawableContainer;
}

// The life of the player walking on the map (GridMovementComponent, mode Keys), whatever its charset : drawn, not
// simulated (its physics shape does not move) :
// - a bob at each foot (BobHeight pixels, more when running), from the distance it really walks (not against a wall)
// - a squash when it starts or stops, a stretch when it hops, a squash when it lands : a spring on the scale of its
//   drawing (anchored at its feet)
// - breathing when it stands still a while
// - dust puffs at its heels when it runs, when it brakes from a run and when it lands (drops on the water)
// It owns the scale and the vertical offset of the drawing effect of the player (GraphicComponent::effect) on the map.
class WalkFeelSystem :
	public ska::System<ska::RequiredComponent<GridMovementComponent, ska::PositionComponent, ska::GraphicComponent, ska::AnimationComponent>, ska::PossibleComponent<>> {
public:
	WalkFeelSystem(ska::EntityManager& entityManager, ska::CameraSystem& camera, const ska::TileWorld& world);
	~WalkFeelSystem() override = default;

	static constexpr float BobHeight = 2.F;
	static constexpr float RunBobHeight = 3.F;
	// Still this long (ms) : it breathes
	static constexpr unsigned int BreathAfter = 1200;
	static constexpr unsigned int PuffLife = 380;

	void graphicUpdate(ska::DrawableContainer& drawables);
	// Its drawing back to normal (ex : a battle starts, its own animations)
	static void reset(ska::GraphicComponent& graphic);

protected:
	void refresh(unsigned int ellapsedTime) override;

private:
	struct Feel {
		ska::Point<long> last;
		bool known = false;
		bool moving = false;
		float speed = 0.F;
		ska::Point<float> heading;
		// Distance walked (pixels), for the feet
		float distance = 0.F;
		int foot = 0;
		unsigned int still = 0;
		long z = 0;
		// The spring of the scale : its stretch (scale y - 1) and its speed
		float stretch = 0.F;
		float stretchSpeed = 0.F;
	};
	struct Puff {
		ska::Point<float> position;
		ska::Point<float> drift;
		unsigned int age = 0;
		bool water = false;
	};

	void puff(ska::Point<float> feet, ska::Point<float> drift, bool water);
	void render(Ui::Canvas& canvas) const;

	ska::CameraSystem& m_camera;
	const ska::TileWorld& m_world;
	float m_blockSize = 48.F;
	std::unordered_map<ska::EntityId, Feel> m_feels;
	std::vector<Puff> m_puffs;
	unsigned int m_seed = 1;
	Ui::Layer m_layer;
};
