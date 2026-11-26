#include <algorithm>
#include <SDL3/SDL.h>
#include "Canvas.h"
#include "Theme.h"
#include "Core/Draw/Renderer.h"
#include "Core/Inputs/InputContextManager.h"
#include "Core/Inputs/InputAction.h"
#include "Core/Inputs/InputRange.h"
#include "Graphic/Texture.h"

void Ui::Canvas::fill(const ska::Color& color, const ska::Rectangle& box) const {
	if (box.w <= 0 || box.h <= 0) {
		return;
	}
	if (color.a < 255) {
		m_renderer.setRenderDrawBlendMode(SDL_BLENDMODE_BLEND);
		m_renderer.drawColorRect(color, box);
		m_renderer.setRenderDrawBlendMode(SDL_BLENDMODE_NONE);
	} else {
		m_renderer.drawColorRect(color, box);
	}
}

int Ui::Canvas::text(const std::string& value, const TextStyle& style, int x, int y, Align align) const {
	if (value.empty()) {
		return 0;
	}
	auto texture = ska::Texture{};
	texture.loadFromText(style.size, value, style.color);
	const auto width = static_cast<int>(texture.getWidth());
	const auto left = align == Align::Left ? x : (align == Align::Center ? x - width / 2 : x - width);
	auto effect = ska::RenderEffect{};
	effect.scaleX = style.scale;
	effect.scaleY = style.scale;
	effect.anchorBottom = false;
	effect.alpha = style.alpha;
	m_renderer.setEffect(&effect);
	if (style.outline > 0) {
		auto outline = ska::Texture{};
		outline.loadFromText(style.size, value, style.outlineColor);
		const auto t = style.outline;
		const int offsets[][2] = { { -t, 0 }, { t, 0 }, { 0, -t }, { 0, t }, { t, t } };
		for (const auto& offset : offsets) {
			m_renderer.render(outline, left + offset[0], y + offset[1], nullptr);
		}
	}
	m_renderer.render(texture, left, y, nullptr);
	m_renderer.setEffect(nullptr);
	return width;
}

void Ui::Canvas::panel(const std::string& image, const ska::Rectangle& box, int corner, unsigned char alpha) const {
	Theme::panel(m_renderer, image, box, corner, alpha);
}

void Ui::Canvas::bar(const ska::Rectangle& box, const Bar& bar) const {
	panel("panel_small_dark.png", ska::Rectangle{ box.x - 4, box.y - 4, box.w + 8, box.h + 8 }, 8);
	const auto width = [&box](float ratio) { return static_cast<int>(static_cast<float>(box.w) * std::clamp(ratio, 0.F, 1.F)); };
	const auto kept = width(bar.ratio);
	const auto trail = width(std::max(bar.ratio, bar.trail));
	const auto from = [&](int w) { return bar.fromRight ? box.x + box.w - w : box.x; };
	fill(bar.trailColor, ska::Rectangle{ from(trail), box.y, trail, box.h });
	fill(bar.fill, ska::Rectangle{ from(kept), box.y, kept, box.h });
	// Lit from the top, as the tiles
	const auto light = std::max(1, (box.h * 5 + 17) / 18);
	const auto shade = std::max(1, box.h * 4 / 18);
	fill(ska::Color{ 255, 255, 255, 110 }, ska::Rectangle{ from(kept), box.y, kept, light });
	fill(ska::Color{ 0, 0, 0, 50 }, ska::Rectangle{ from(kept), box.y + box.h - shade, kept, shade });
}

void Ui::Canvas::image(const ska::Texture& texture, int x, int y, const ska::Rectangle* clip, float scale, unsigned char alpha, bool anchorBottom) const {
	auto effect = ska::RenderEffect{};
	effect.scaleX = scale;
	effect.scaleY = scale;
	effect.alpha = alpha;
	effect.anchorBottom = anchorBottom;
	m_renderer.setEffect(&effect);
	m_renderer.render(texture, x, y, clip);
	m_renderer.setEffect(nullptr);
}

Ui::Layer::Layer(std::function<void(Canvas&)> draw, int priority) : m_draw(std::move(draw)) {
	setPriority(priority);
}

void Ui::Layer::render(ska::Renderer& renderer) const {
	auto canvas = Canvas{ renderer };
	m_draw(canvas);
}

void Ui::ChoiceList::setCount(std::size_t count) {
	m_count = count;
	m_selected = count == 0 ? 0 : std::min(m_selected, count - 1);
}

bool Ui::ChoiceList::keys(const ska::InputContextManager& icm) {
	const auto& toggles = icm.getToggles();
	const auto& actions = icm.getActions();
	const auto pressed = [&](ska::InputToggle toggle) { return toggles[toggle] && !m_lastToggles[toggle]; };
	const auto action = actions[ska::DoAction] || actions[ska::Jump];
	auto chosen = false;
	if (m_count > 0) {
		if (pressed(ska::MoveUp)) {
			m_selected = (m_selected + m_count - 1) % m_count;
		} else if (pressed(ska::MoveDown)) {
			m_selected = (m_selected + 1) % m_count;
		} else if (action && !m_lastAction) {
			chosen = true;
		}
	}
	m_lastToggles = toggles;
	m_lastAction = action;
	return chosen;
}

bool Ui::ChoiceList::mouse(const ska::InputContextManager& icm, const std::vector<ska::Rectangle>& boxes) {
	const auto& position = icm.getRanges()[ska::MousePos];
	const auto click = icm.getActions()[ska::LClic];
	auto chosen = false;
	// (0, 0) : no mouse yet (a touch screen, a headless run)
	if (position.x != 0 || position.y != 0) {
		for (auto i = 0u; i < boxes.size() && i < m_count; i++) {
			const auto& box = boxes[i];
			if (position.x >= box.x && position.x < box.x + box.w && position.y >= box.y && position.y < box.y + box.h) {
				m_selected = i;
				chosen = click && !m_lastClick;
				break;
			}
		}
	}
	m_lastClick = click;
	return chosen;
}
