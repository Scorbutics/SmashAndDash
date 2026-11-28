#include "BattleAuras.h"
#include <algorithm>
#include <cmath>
#include <SDL3/SDL.h>
#include "Canvas.h"
#include "Core/Draw/Renderer.h"

namespace {
	constexpr auto Pi = 3.14159265F;

	// A number 0 .. 1 from integers (the same each frame : the particles keep their place in their cycle)
	float Hash(int a, int b) {
		auto h = static_cast<unsigned int>(a) * 374761393U + static_cast<unsigned int>(b) * 668265263U;
		h = (h ^ (h >> 13U)) * 1274126177U;
		return static_cast<float>((h ^ (h >> 16U)) & 0xFFFFU) / 65535.F;
	}

	unsigned char Byte(float value) {
		return static_cast<unsigned char>(std::max(0.F, std::min(255.F, value)));
	}

	ska::Color Rgba(int r, int g, int b, float a) {
		return ska::Color{ static_cast<uint8_t>(r), static_cast<uint8_t>(g), static_cast<uint8_t>(b), Byte(a) };
	}

	// A disc of rows of pixels
	void Disc(const Ui::Canvas& canvas, float cx, float cy, float radius, const ska::Color& color) {
		const auto r = std::max(1, static_cast<int>(radius));
		for (auto dy = -r; dy <= r; dy++) {
			const auto half = static_cast<int>(std::sqrt(static_cast<float>(r * r - dy * dy)));
			canvas.fill(color, ska::Rectangle{ static_cast<int>(cx) - half, static_cast<int>(cy) + dy, half * 2 + 1, 1 });
		}
	}

	void Line(const Ui::Canvas& canvas, float x0, float y0, float x1, float y1, const ska::Color& color, int thickness) {
		auto& renderer = canvas.renderer();
		for (auto k = 0; k < thickness; k++) {
			const auto o = k - thickness / 2;
			renderer.drawColorLine(color, ska::Point<int>{ static_cast<int>(x0) + o, static_cast<int>(y0) }, ska::Point<int>{ static_cast<int>(x1) + o, static_cast<int>(y1) });
			renderer.drawColorLine(color, ska::Point<int>{ static_cast<int>(x0), static_cast<int>(y0) + o }, ska::Point<int>{ static_cast<int>(x1), static_cast<int>(y1) + o });
		}
	}

	// Purple bubbles from its body that rise, wobble and pop
	void Poison(const Ui::Canvas& canvas, ska::Point<float> feet, float body, unsigned int time, float burst) {
		const auto px = std::max(1.F, body / 32.F);
		const auto count = 6 + static_cast<int>(6.F * burst);
		for (auto i = 0; i < count; i++) {
			const auto period = 1300.F + 500.F * Hash(i, 1);
			const auto t = std::fmod(static_cast<float>(time) + Hash(i, 2) * period, period) / period;
			const auto x = feet.x + (Hash(i, 3) - 0.5F) * body * 0.9F + std::sin(t * 9.F + static_cast<float>(i)) * px * 2.F;
			const auto y = feet.y - body * 0.15F - t * body * 1.2F;
			const auto popping = t > 0.86F;
			const auto radius = (1.5F + 2.5F * Hash(i, 4)) * px * (popping ? 1.F + (t - 0.86F) * 6.F : 0.6F + t);
			const auto alpha = 230.F * (popping ? 1.F - (t - 0.86F) / 0.14F : std::min(1.F, t * 6.F));
			if (popping) {
				// The bubble bursts : a ring
				for (auto a = 0; a < 10; a++) {
					const auto angle = static_cast<float>(a) * 2.F * Pi / 10.F;
					canvas.fill(Rgba(220, 150, 255, alpha), ska::Rectangle{ static_cast<int>(x + std::cos(angle) * radius), static_cast<int>(y + std::sin(angle) * radius), static_cast<int>(px), static_cast<int>(px) });
				}
				continue;
			}
			Disc(canvas, x, y, radius, Rgba(150, 60, 200, alpha));
			Disc(canvas, x, y, std::max(1.F, radius - px), Rgba(190, 100, 240, alpha));
			canvas.fill(Rgba(255, 230, 255, alpha), ska::Rectangle{ static_cast<int>(x - radius * 0.4F), static_cast<int>(y - radius * 0.5F), static_cast<int>(std::max(1.F, px)), static_cast<int>(std::max(1.F, px)) });
		}
	}

	// Flames : squares that rise from its body, yellow then orange then red, shrinking
	void Burn(const Ui::Canvas& canvas, ska::Point<float> feet, float body, unsigned int time, float burst) {
		const auto px = std::max(1.F, body / 32.F);
		const auto count = 9 + static_cast<int>(9.F * burst);
		for (auto i = 0; i < count; i++) {
			const auto period = 520.F + 380.F * Hash(i, 11);
			const auto t = std::fmod(static_cast<float>(time) + Hash(i, 12) * period, period) / period;
			const auto x = feet.x + (Hash(i, 13) - 0.5F) * body * 0.8F + std::sin(t * 7.F + static_cast<float>(i) * 1.3F) * px * 1.5F;
			const auto y = feet.y - body * (0.05F + 0.35F * Hash(i, 14)) - t * body * 0.9F;
			const auto size = (2.F + 3.F * Hash(i, 15)) * px * (1.F - t * 0.8F);
			const auto color = t < 0.3F ? Rgba(255, 240, 130, 240.F) : (t < 0.65F ? Rgba(255, 140, 40, 220.F) : Rgba(210, 50, 20, 200.F * (1.F - (t - 0.65F) / 0.35F)));
			canvas.fill(color, ska::Rectangle{ static_cast<int>(x - size / 2.F), static_cast<int>(y - size / 2.F), static_cast<int>(size) + 1, static_cast<int>(size) + 1 });
		}
	}

	// Sparks that crackle around it, a few times a second
	void Paralysis(const Ui::Canvas& canvas, ska::Point<float> feet, float body, unsigned int time, float burst) {
		const auto px = std::max(1.F, body / 32.F);
		const auto cycle = static_cast<int>(time / 450U);
		const auto within = static_cast<float>(time % 450U);
		const auto shown = burst > 0.F ? 260.F : 150.F;
		if (within > shown) {
			return;
		}
		const auto alpha = 255.F * (1.F - within / shown);
		auto& renderer = canvas.renderer();
		renderer.setRenderDrawBlendMode(SDL_BLENDMODE_BLEND);
		const auto bolts = 2 + static_cast<int>(2.F * burst);
		for (auto b = 0; b < bolts; b++) {
			// A bolt : a zigzag from a point of its outline outwards
			const auto angle = Hash(cycle, b * 7 + 1) * 2.F * Pi;
			auto x = feet.x + std::cos(angle) * body * 0.35F;
			auto y = feet.y - body * 0.5F + std::sin(angle) * body * 0.4F;
			for (auto s = 0; s < 4; s++) {
				const auto nx = x + std::cos(angle) * px * 4.F + (Hash(cycle, b * 7 + s + 2) - 0.5F) * px * 7.F;
				const auto ny = y + std::sin(angle) * px * 4.F + (Hash(cycle, b * 7 + s + 30) - 0.5F) * px * 7.F;
				Line(canvas, x, y, nx, ny, Rgba(255, 245, 90, alpha), std::max(1, static_cast<int>(px)));
				x = nx;
				y = ny;
			}
			canvas.fill(Rgba(255, 255, 220, alpha), ska::Rectangle{ static_cast<int>(x - px), static_cast<int>(y - px), static_cast<int>(px * 2.F), static_cast<int>(px * 2.F) });
		}
		renderer.setRenderDrawBlendMode(SDL_BLENDMODE_NONE);
	}

	// "Z" that rise from its head, growing and fading
	void Sleep(const Ui::Canvas& canvas, ska::Point<float> feet, float body, unsigned int time) {
		const auto px = std::max(1.F, body / 32.F);
		for (auto i = 0; i < 3; i++) {
			const auto period = 1800.F;
			const auto t = std::fmod(static_cast<float>(time) + static_cast<float>(i) * period / 3.F, period) / period;
			const auto x = feet.x + body * 0.25F + t * body * 0.5F + std::sin(t * 6.F) * px * 2.F;
			const auto y = feet.y - body * 1.05F - t * body * 0.8F;
			auto style = Ui::TextStyle{};
			style.size = static_cast<unsigned int>(std::max(10.F, 9.F * px));
			style.color = ska::Color{ 235, 240, 255, 255 };
			style.outline = std::max(1, static_cast<int>(px * 0.7F));
			style.outlineColor = ska::Color{ 60, 70, 140, 255 };
			style.scale = 0.6F + 0.8F * t;
			style.alpha = Byte(255.F * std::min(1.F, t * 5.F) * (1.F - std::max(0.F, (t - 0.7F) / 0.3F)));
			canvas.text("Z", style, static_cast<int>(x), static_cast<int>(y), Ui::Align::Center);
		}
	}

	// Seeds that turn around it ; its life drained upwards (green motes)
	void Leech(const Ui::Canvas& canvas, ska::Point<float> feet, float body, unsigned int time, float burst) {
		const auto px = std::max(1.F, body / 32.F);
		const auto turn = static_cast<float>(time) / 900.F;
		for (auto i = 0; i < 5; i++) {
			const auto angle = turn + static_cast<float>(i) * 2.F * Pi / 5.F;
			const auto x = feet.x + std::cos(angle) * body * 0.55F;
			const auto y = feet.y - body * 0.35F + std::sin(angle) * body * 0.18F;
			// A seed : brown, a green leaf on it
			canvas.fill(Rgba(60, 40, 20, 240.F), ska::Rectangle{ static_cast<int>(x - px * 2.5F), static_cast<int>(y - px * 1.5F), static_cast<int>(px * 5.F), static_cast<int>(px * 3.F) });
			canvas.fill(Rgba(150, 100, 50, 250.F), ska::Rectangle{ static_cast<int>(x - px * 2.F), static_cast<int>(y - px), static_cast<int>(px * 4.F), static_cast<int>(px * 2.F) });
			canvas.fill(Rgba(90, 220, 90, 250.F), ska::Rectangle{ static_cast<int>(x), static_cast<int>(y - px * 3.F), static_cast<int>(px * 3.F), static_cast<int>(px * 1.5F) + 1 });
		}
		const auto motes = 3 + static_cast<int>(8.F * burst);
		for (auto i = 0; i < motes; i++) {
			const auto period = 1000.F;
			const auto t = std::fmod(static_cast<float>(time) + Hash(i, 21) * period, period) / period;
			const auto x = feet.x + (Hash(i, 22) - 0.5F) * body * 0.7F;
			const auto y = feet.y - body * 0.3F - t * body * 1.1F;
			const auto size = (1.5F + 2.F * Hash(i, 23)) * px;
			canvas.fill(Rgba(150, 255, 140, 220.F * (1.F - t)), ska::Rectangle{ static_cast<int>(x), static_cast<int>(y), static_cast<int>(size) + 1, static_cast<int>(size) + 1 });
		}
	}
}

void BattleAuras::status(const Ui::Canvas& canvas, FighterStatus status, ska::Point<float> feet, float body, unsigned int time, float burst) {
	burst = std::max(0.F, std::min(1.F, burst));
	switch (status) {
	case FighterStatus::Poison: Poison(canvas, feet, body, time, burst); break;
	case FighterStatus::Burn: Burn(canvas, feet, body, time, burst); break;
	case FighterStatus::Paralysis: Paralysis(canvas, feet, body, time, burst); break;
	case FighterStatus::Sleep: Sleep(canvas, feet, body, time); break;
	case FighterStatus::Leech: Leech(canvas, feet, body, time, burst); break;
	default: break;
	}
}

void BattleAuras::statChange(const Ui::Canvas& canvas, bool up, ska::Point<float> feet, float body, float progress) {
	if (progress <= 0.F || progress >= 1.F) {
		return;
	}
	const auto px = std::max(1.F, body / 32.F);
	// It fades in, stays, fades out
	const auto alpha = 230.F * std::min(1.F, progress * 6.F) * std::min(1.F, (1.F - progress) * 4.F);
	const auto color = up ? Rgba(255, 120, 80, alpha) : Rgba(90, 160, 255, alpha);
	const auto light = up ? Rgba(255, 220, 190, alpha * 0.8F) : Rgba(200, 230, 255, alpha * 0.8F);
	const auto top = feet.y - body * 1.25F;
	const auto height = body * 1.25F;
	const auto left = feet.x - body * 0.6F;
	const auto width = body * 1.2F;
	auto& renderer = canvas.renderer();
	renderer.setRenderDrawBlendMode(SDL_BLENDMODE_BLEND);
	// Columns of chevrons that scroll the way it changed, kept inside its body
	const auto spacing = 9.F * px;
	const auto scroll = std::fmod(progress * height * 2.2F, spacing);
	const auto half = 3.5F * px;
	for (auto column = 0; column < 3; column++) {
		const auto x = left + width * (0.2F + 0.3F * static_cast<float>(column));
		const auto stagger = static_cast<float>(column % 2) * spacing * 0.5F;
		for (auto row = -1; row < static_cast<int>(height / spacing) + 1; row++) {
			auto y = top + static_cast<float>(row) * spacing + stagger + (up ? -scroll : scroll);
			if (y < top || y > top + height - half) {
				continue;
			}
			// A chevron pointing the way it goes
			const auto tip = up ? -half : half;
			Line(canvas, x - half, y, x, y + tip, color, std::max(1, static_cast<int>(px * 1.3F)));
			Line(canvas, x, y + tip, x + half, y, color, std::max(1, static_cast<int>(px * 1.3F)));
		}
		// A streak along the column
		const auto streak = up ? top + height * (1.F - progress) : top + height * progress;
		canvas.fill(light, ska::Rectangle{ static_cast<int>(x) - static_cast<int>(px / 2.F), static_cast<int>(streak - 6.F * px), std::max(1, static_cast<int>(px)), static_cast<int>(12.F * px) });
	}
	renderer.setRenderDrawBlendMode(SDL_BLENDMODE_NONE);
}
