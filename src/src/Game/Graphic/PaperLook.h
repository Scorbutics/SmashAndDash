#pragma once
#include "Core/Draw/DrawableContainer.h"
#include "Core/Draw/DrawableFixedPriority.h"
#include "Graphic/Shader.h"

namespace ska {
	class Renderer;
	class CameraSystem;
}

// The world painted on paper (Shaders/paper, src/shaders/paper.frag) : the shader of the world (Renderer::setWorldShader :
// the map and its characters, not the interface), set at each frame while the map is drawn, removed with the map.
// Its strength : SAD_PAPER (0 : off, 1 : as designed, up to 2). Without the SDL3 gpu renderer (the web) : nothing.
class PaperLook {
public:
	explicit PaperLook(ska::CameraSystem& camera);
	PaperLook(const PaperLook&) = delete;
	PaperLook& operator=(const PaperLook&) = delete;
	~PaperLook();

	// Added to the drawings of each frame of the map
	void graphicUpdate(ska::DrawableContainer& drawables);

private:
	class Hook : public ska::DrawableFixedPriority {
	public:
		explicit Hook(PaperLook& look) : m_look(look) {}
		void render(ska::Renderer& renderer) const override;
	private:
		PaperLook& m_look;
	};

	ska::CameraSystem& m_camera;
	float m_strength = 1.F;
	ska::Shader m_shader;
	Hook m_hook;
	mutable ska::Renderer* m_renderer = nullptr;
};
