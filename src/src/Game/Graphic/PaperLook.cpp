#include "Game/__internalConfig/LoggerConfig.h"
#include "PaperLook.h"
#include <algorithm>
#include <cstdlib>
#include "Core/Draw/Renderer.h"
#include "Graphic/System/CameraSystem.h"

PaperLook::PaperLook(ska::CameraSystem& camera) :
	m_camera(camera),
	m_shader("./Shaders/paper", true),
	m_hook(*this) {
	const auto* env = std::getenv("SAD_PAPER");
	if (env != nullptr) {
		m_strength = std::clamp(static_cast<float>(std::atof(env)), 0.F, 2.F);
	}
	// The first of the drawings of the world : the shader is set before the world goes on the screen
	m_hook.setPriority(-1000000);
}

PaperLook::~PaperLook() {
	if (m_renderer != nullptr) {
		m_renderer->setWorldShader(nullptr);
	}
}

void PaperLook::graphicUpdate(ska::DrawableContainer& drawables) {
	if (m_strength > 0.F) {
		drawables.add(m_hook);
	}
}

void PaperLook::Hook::render(ska::Renderer& renderer) const {
	m_look.m_renderer = &renderer;
	const auto s = m_look.m_strength;
	const auto screen = m_look.m_camera.getScreenSize();
	struct {
		float paper[4];
		float screen[4];
	} uniforms{ { s, s, s, s }, { static_cast<float>(screen.x), static_cast<float>(screen.y), 7.13F, 0.F } };
	m_look.m_shader.setUniforms(uniforms);
	renderer.setWorldShader(&m_look.m_shader);
}
