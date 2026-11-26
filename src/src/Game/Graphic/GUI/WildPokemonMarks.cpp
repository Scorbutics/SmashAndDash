#include "Game/__internalConfig/LoggerConfig.h"
#include "WildPokemonMarks.h"
#include <algorithm>
#include <cmath>
#include "Core/Draw/DrawableContainer.h"
#include "Graphic/System/CameraSystem.h"
#include "../../Utils/IDs.h"

WildPokemonMarks::WildPokemonMarks(ska::EntityManager& entityManager, ska::CameraSystem& camera) :
	System(entityManager),
	m_camera(camera),
	m_layer([this](Ui::Canvas& canvas) { render(canvas); }, WorldOverlayPriority) {
}

void WildPokemonMarks::graphicUpdate(ska::DrawableContainer& drawables) {
	drawables.add(m_layer);
}

void WildPokemonMarks::render(Ui::Canvas& canvas) {
	const auto& camera = m_camera.getDisplay();
	for (const auto entity : getEntities()) {
		const auto& grid = m_componentAccessor.get<GridMovementComponent>(entity);
		const auto& wild = m_componentAccessor.get<WildPokemonComponent>(entity);
		if (!grid.seeing || grid.seen >= Duration || wild.calm > 0 || wild.attitude == WildPokemonComponent::Attitude::Passive) {
			continue;
		}
		// Over its sprite, as the GraphicSystem draws it
		auto& graphic = m_componentAccessor.get<ska::GraphicComponent>(entity);
		const auto& hc = m_componentAccessor.get<ska::HitboxComponent>(entity);
		const auto drawn = graphic.drawnPosition(m_componentAccessor.get<ska::PositionComponent>(entity));
		const auto width = graphic.animatedSprites.empty() ? 0 : static_cast<int>(graphic.animatedSprites[0].getWidth());
		const auto x = static_cast<int>(std::lround(drawn.x)) - static_cast<int>(hc.width / 2) + hc.xOffset / 2 - camera.x + width / 2;
		const auto y = static_cast<int>(std::lround(drawn.y)) - static_cast<int>(hc.height / 2) - 5 - camera.y - static_cast<int>(std::lround(drawn.z));

		// It pops (big, then normal, a small jump), then fades
		const auto age = grid.seen;
		const auto scale = age < 120 ? 1.8F - 0.8F * static_cast<float>(age) / 120.F : 1.F;
		const auto jump = age < 220 ? static_cast<int>(8.F * std::sin(static_cast<float>(age) / 220.F * 3.1416F)) : 0;
		const auto alpha = age > Duration - 300 ? static_cast<unsigned char>(255 * (Duration - age) / 300) : static_cast<unsigned char>(255);
		const auto aggressive = wild.attitude == WildPokemonComponent::Attitude::Aggressive;
		auto style = Ui::TextStyle{};
		style.size = 22;
		style.color = aggressive ? ska::Color{ 255, 80, 60, 255 } : ska::Color{ 120, 200, 255, 255 };
		style.outline = 2;
		style.scale = scale;
		style.alpha = alpha;
		canvas.text(aggressive ? "!" : "?!", style, x, y - 26 - jump, Ui::Align::Center);
	}
}
