#pragma once
#include <functional>
#include "Core/Draw/DrawableFixedPriority.h"
#include "Core/Point.h"

// The screen darkens, an action is done when it is black (ex : the map changes), then the screen lightens (as the
// transitions of the classic Pokemon games). Drawn over everything.
class ScreenFade : public ska::DrawableFixedPriority {
public:
	ScreenFade();
	~ScreenFade() override = default;

	// Starts a transition : false when one is already running (the action is not done)
	bool start(std::function<void()> atBlack);
	void update(unsigned int ellapsedTime);
	// The screen is darkening, black or lightening : the player waits
	bool running() const { return m_phase != Phase::None; }
	void setScreenSize(ska::Point<int> size) { m_screenSize = size; }

	void render(ska::Renderer& renderer) const override;

	static constexpr unsigned int OutDuration = 220;
	static constexpr unsigned int BlackDuration = 120;
	static constexpr unsigned int InDuration = 280;

private:
	enum class Phase { None, Out, Black, In };
	Phase m_phase = Phase::None;
	unsigned int m_time = 0;
	std::function<void()> m_atBlack;
	ska::Point<int> m_screenSize;
	float m_darkness = 0.F;
};
