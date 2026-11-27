#include <algorithm>
#include <chrono>
#include <cstdint>
#include <unordered_map>
#include <SDL3/SDL.h>
#include "Canvas.h"
#include "Theme.h"
#include "Core/Draw/Renderer.h"
#include "Core/Inputs/InputContextManager.h"
#include "Core/Inputs/InputAction.h"
#include "Core/Inputs/InputRange.h"
#include "Graphic/Texture.h"
#include "Core/Dev/Profiler.h"

namespace {
	const std::string TextPart = "ui text";
	const std::string PanelPart = "ui panel";
	const std::string ImagePart = "ui image";
	const std::string FillPart = "ui fill";

	// The texts drawn, kept while they are drawn (a text made from the font costs about a millisecond : the screens that
	// draw themselves each frame show tens of them) ; forgotten 3 s after their last drawing
	struct TextKey {
		std::string value;
		unsigned int size;
		std::uint32_t color;
		bool operator==(const TextKey& other) const { return size == other.size && color == other.color && value == other.value; }
	};
	struct TextKeyHash {
		std::size_t operator()(const TextKey& key) const {
			return std::hash<std::string>{}(key.value) ^ (static_cast<std::size_t>(key.size) * 0x9E3779B1U) ^ (static_cast<std::size_t>(key.color) << 7);
		}
	};
	struct CachedText {
		ska::Texture texture;
		std::chrono::steady_clock::time_point used;
	};

	const ska::Texture& Text(unsigned int size, const std::string& value, const ska::Color& color) {
		static auto cache = std::unordered_map<TextKey, CachedText, TextKeyHash>{};
		static auto lastPurge = std::chrono::steady_clock::now();
		const auto now = std::chrono::steady_clock::now();
		if (now - lastPurge > std::chrono::seconds(1)) {
			lastPurge = now;
			for (auto it = cache.begin(); it != cache.end();) {
				it = now - it->second.used > std::chrono::seconds(3) ? cache.erase(it) : std::next(it);
			}
		}
		const auto key = TextKey{ value, size, (static_cast<std::uint32_t>(color.r) << 24) | (static_cast<std::uint32_t>(color.g) << 16) | (static_cast<std::uint32_t>(color.b) << 8) | color.a };
		auto found = cache.find(key);
		if (found == cache.end()) {
			auto entry = CachedText{};
			entry.texture.loadFromText(size, value, color);
			found = cache.emplace(key, std::move(entry)).first;
		}
		found->second.used = now;
		return found->second.texture;
	}
}

void Ui::Canvas::fill(const ska::Color& color, const ska::Rectangle& box) const {
	if (box.w <= 0 || box.h <= 0) {
		return;
	}
	const auto profiled = ska::Profiler::Scope{ FillPart };
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
	const auto profiled = ska::Profiler::Scope{ TextPart };
	const auto& texture = Text(style.size, value, style.color);
	const auto width = static_cast<int>(texture.getWidth());
	const auto left = align == Align::Left ? x : (align == Align::Center ? x - width / 2 : x - width);
	auto effect = ska::RenderEffect{};
	effect.scaleX = style.scale;
	effect.scaleY = style.scale;
	effect.anchorBottom = false;
	effect.alpha = style.alpha;
	m_renderer.setEffect(&effect);
	if (style.outline > 0) {
		const auto& outline = Text(style.size, value, style.outlineColor);
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
	const auto profiled = ska::Profiler::Scope{ PanelPart };
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
	const auto profiled = ska::Profiler::Scope{ ImagePart };
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
