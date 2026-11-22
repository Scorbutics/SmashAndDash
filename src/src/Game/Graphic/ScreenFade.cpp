#include <algorithm>
#include <climits>
#include <SDL3/SDL.h>
#include "ScreenFade.h"
#include "Core/Draw/Renderer.h"

ScreenFade::ScreenFade() {
	// Over the interface too (the mouse cursor excepted)
	setPriority(INT_MAX - 1);
}

bool ScreenFade::start(std::function<void()> atBlack) {
	if (m_phase != Phase::None) {
		return false;
	}
	m_phase = Phase::Out;
	m_time = 0;
	m_atBlack = std::move(atBlack);
	return true;
}

void ScreenFade::update(unsigned int ellapsedTime) {
	if (m_phase == Phase::None) {
		m_darkness = 0.F;
		return;
	}
	// A long frame (ex : the loading of a map) does not skip the transition
	m_time += std::min(ellapsedTime, 40u);
	switch (m_phase) {
	case Phase::Out:
		m_darkness = std::min(1.F, static_cast<float>(m_time) / static_cast<float>(OutDuration));
		if (m_time >= OutDuration) {
			m_darkness = 1.F;
			m_phase = Phase::Black;
			m_time = 0;
			auto action = std::move(m_atBlack);
			m_atBlack = nullptr;
			if (action) {
				action();
			}
		}
		break;
	case Phase::Black:
		m_darkness = 1.F;
		if (m_time >= BlackDuration) {
			m_phase = Phase::In;
			m_time = 0;
		}
		break;
	case Phase::In:
		m_darkness = 1.F - std::min(1.F, static_cast<float>(m_time) / static_cast<float>(InDuration));
		if (m_time >= InDuration) {
			m_darkness = 0.F;
			m_phase = Phase::None;
		}
		break;
	default:
		break;
	}
}

void ScreenFade::render(ska::Renderer& renderer) const {
	if (m_darkness <= 0.F) {
		return;
	}
	renderer.setRenderDrawBlendMode(SDL_BLENDMODE_BLEND);
	const auto alpha = static_cast<uint8_t>(std::clamp(m_darkness, 0.F, 1.F) * 255.F);
	renderer.drawColorRect(ska::Color{ 0, 0, 0, alpha }, ska::Rectangle{ 0, 0, m_screenSize.x, m_screenSize.y });
	renderer.setRenderDrawBlendMode(SDL_BLENDMODE_NONE);
}
