#include "Game/__internalConfig/LoggerConfig.h"
#include "ClashArena.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <filesystem>
#include <SDL3/SDL.h>
#include "Core/Draw/Renderer.h"
#include "Core/ECS/EntityManager.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Core/Utils/SpritePath.h"
#include "Core/Utils/SkaConstants.h"
#include "Graphic/GraphicComponent.h"
#include "Graphic/System/CameraSystem.h"
#include "World/TileWorld.h"
#include "World/Tileset.h"
#include "../../Gameplay/Fight/BattleComponent.h"
#include "../../Gameplay/Fight/FighterComponent.h"
#include "../../Gameplay/World/WorldState.h"
#include "../../Gameplay/World/Generation/Png.h"

namespace {
	// The blocks kept around the Pokemon of the player (each side), the blocks that break around the clash
	constexpr int Radius = 18;
	constexpr float ShatterRadius = 10.F;
	// The battleground in perspective (as the racing games of the arcades) : the ground from "NearRow" blocks behind the
	// Pokemon of the player to "FarRow" blocks in front, "SideBlocks" each side ; the camera "CameraHeight" blocks high,
	// the Pokemon of the player "PlayerDepth" blocks in front of it
	constexpr int NearRow = -1;
	constexpr int FarRow = 17;
	constexpr int SideBlocks = 22;
	constexpr float CameraHeight = 1.5F;
	constexpr float PlayerDepth = 2.5F;
	constexpr float Pi = 3.14159265F;

	using Step = BattleTurn::Step;

	float Clamp01(float value) { return std::clamp(value, 0.F, 1.F); }
	float EaseOut(float t) { t = Clamp01(t); return 1.F - (1.F - t) * (1.F - t) * (1.F - t); }
	float EaseIn(float t) { t = Clamp01(t); return t * t; }
	float EaseOutBack(float t) {
		t = Clamp01(t);
		constexpr auto c = 1.9F;
		const auto u = t - 1.F;
		return 1.F + (c + 1.F) * u * u * u + c * u * u;
	}
	float Bounce(float t) {
		t = Clamp01(t);
		if (t < 0.55F) {
			return EaseIn(t / 0.55F);
		}
		const auto u = (t - 0.55F) / 0.45F;
		return 1.F - 0.18F * std::sin(u * Pi) * (1.F - u);
	}
	float Lerp(float a, float b, float t) { return a + (b - a) * t; }
	unsigned char Byte(float value) { return static_cast<unsigned char>(std::clamp(value, 0.F, 255.F)); }

	// A number in 0 .. 1 from integers : the drawing does not roll the dice of the battle (its replays stay the same)
	float Hash(int a, int b, int c = 0) {
		auto h = static_cast<unsigned int>(a) * 374761393U + static_cast<unsigned int>(b) * 668265263U + static_cast<unsigned int>(c) * 2147483647U;
		h = (h ^ (h >> 13)) * 1274126177U;
		h ^= h >> 16;
		return static_cast<float>(h & 0xFFFFU) / 65535.F;
	}

	// Upper case (UTF-8) : the ASCII letters and the accented Latin letters
	std::string Upper(std::string text) {
		for (auto i = std::size_t{ 0 }; i < text.size(); i++) {
			const auto u = static_cast<unsigned char>(text[i]);
			if (u < 0x80) {
				text[i] = static_cast<char>(std::toupper(u));
			} else if (u == 0xC3 && i + 1 < text.size()) {
				const auto next = static_cast<unsigned char>(text[i + 1]);
				if (next >= 0xA0 && next <= 0xBE && next != 0xB7) {
					text[i + 1] = static_cast<char>(next - 0x20);
				}
				i++;
			}
		}
		return text;
	}

	ska::Color Rgba(int r, int g, int b, float alpha = 255.F) {
		return ska::Color{ Byte(static_cast<float>(r)), Byte(static_cast<float>(g)), Byte(static_cast<float>(b)), Byte(alpha) };
	}

	// The colours of the types of the skills (as their data names them)
	ska::Color TypeColor(const std::string& type) {
		if (type == "Feu") { return Rgba(236, 92, 48); }
		if (type == "Eau") { return Rgba(64, 130, 236); }
		if (type == "Plante") { return Rgba(78, 178, 76); }
		if (type == "Electrique") { return Rgba(232, 190, 30); }
		if (type == "Vol") { return Rgba(120, 150, 230); }
		if (type == "Poison") { return Rgba(160, 70, 170); }
		if (type == "Sol") { return Rgba(190, 150, 70); }
		if (type == "Insecte") { return Rgba(150, 180, 40); }
		if (type == "Tenebres") { return Rgba(90, 70, 80); }
		if (type == "Dragon") { return Rgba(100, 60, 220); }
		if (type == "Acier") { return Rgba(150, 160, 180); }
		return Rgba(150, 140, 120);
	}

	Ui::TextStyle Style(unsigned int size, ska::Color color, int outline, ska::Color outlineColor, float scale = 1.F, float alpha = 255.F) {
		auto style = Ui::TextStyle{};
		style.size = size;
		style.color = color;
		style.outline = outline;
		style.outlineColor = outlineColor;
		style.scale = scale;
		style.alpha = Byte(alpha);
		return style;
	}

	// A big text of the fighting games : its centre at x y, a drop shadow, a thick outline
	void Shout(const Ui::Canvas& canvas, const std::string& text, unsigned int size, ska::Color color, ska::Color outline, float x, float y, float scale, float alpha) {
		if (alpha <= 0.F || text.empty()) {
			return;
		}
		const auto top = static_cast<int>(y - static_cast<float>(size) * 0.62F);
		const auto shadow = std::max(2, static_cast<int>(static_cast<float>(size) / 16.F));
		canvas.text(text, Style(size, Rgba(0, 0, 0), 0, Rgba(0, 0, 0), scale, alpha * 0.6F), static_cast<int>(x) + shadow, top + shadow, Ui::Align::Center);
		canvas.text(text, Style(size, color, std::max(2, static_cast<int>(size) / 18), outline, scale, alpha), static_cast<int>(x), top, Ui::Align::Center);
	}

	// A line with transparency, "thickness" lines side by side
	void Line(const Ui::Canvas& canvas, ska::Color color, float x1, float y1, float x2, float y2, int thickness = 1) {
		auto& renderer = canvas.renderer();
		renderer.setRenderDrawBlendMode(SDL_BLENDMODE_BLEND);
		const auto dx = x2 - x1;
		const auto dy = y2 - y1;
		const auto length = std::max(0.001F, std::sqrt(dx * dx + dy * dy));
		const auto nx = -dy / length;
		const auto ny = dx / length;
		for (auto i = 0; i < thickness; i++) {
			const auto o = static_cast<float>(i) - static_cast<float>(thickness - 1) / 2.F;
			renderer.drawColorLine(color, ska::Point<int>{ static_cast<int>(x1 + nx * o), static_cast<int>(y1 + ny * o) }, ska::Point<int>{ static_cast<int>(x2 + nx * o), static_cast<int>(y2 + ny * o) });
		}
		renderer.setRenderDrawBlendMode(SDL_BLENDMODE_NONE);
	}

	void Ellipse(const Ui::Canvas& canvas, float cx, float cy, float rx, float ry, ska::Color color) {
		if (rx <= 0.F || ry <= 0.F) {
			return;
		}
		for (auto dy = -ry; dy <= ry; dy += 2.F) {
			const auto w = rx * std::sqrt(std::max(0.F, 1.F - (dy / ry) * (dy / ry)));
			canvas.fill(color, ska::Rectangle{ static_cast<int>(cx - w), static_cast<int>(cy + dy), static_cast<int>(2.F * w), 2 });
		}
	}

	// A box slanted as the boxes of the fighting games (its top shifted right by "slant")
	void Slanted(const Ui::Canvas& canvas, float x, float y, float w, float h, float slant, ska::Color color) {
		for (auto row = 0.F; row < h; row += 2.F) {
			const auto shift = slant * (1.F - row / h);
			canvas.fill(color, ska::Rectangle{ static_cast<int>(x + shift), static_cast<int>(y + row), static_cast<int>(w), 2 });
		}
	}

	// A jagged bolt from a to b
	void Bolt(const Ui::Canvas& canvas, float ax, float ay, float bx, float by, float jitter, int seed, ska::Color glow, float alpha) {
		constexpr auto Segments = 9;
		const auto dx = bx - ax;
		const auto dy = by - ay;
		const auto length = std::max(0.001F, std::sqrt(dx * dx + dy * dy));
		const auto nx = -dy / length;
		const auto ny = dx / length;
		auto px = ax;
		auto py = ay;
		for (auto i = 1; i <= Segments; i++) {
			const auto t = static_cast<float>(i) / static_cast<float>(Segments);
			const auto off = i == Segments ? 0.F : (Hash(seed, i) - 0.5F) * 2.F * jitter;
			const auto qx = ax + dx * t + nx * off;
			const auto qy = ay + dy * t + ny * off;
			Line(canvas, Rgba(glow.r, glow.g, glow.b, alpha * 0.55F), px, py, qx, qy, 7);
			Line(canvas, Rgba(255, 255, 255, alpha), px, py, qx, qy, 3);
			px = qx;
			py = qy;
		}
	}
}

ClashArena::ClashArena(ska::EntityManager& em, const BattleStage& stage, ska::CameraSystem& camera, WorldState& worldState,
	std::function<const ska::Texture&(int)> skillSprite, const ska::Texture& impact) :
	m_entities(em),
	m_stage(stage),
	m_camera(camera),
	m_worldState(worldState),
	m_skillSprite(std::move(skillSprite)),
	m_impact(impact) {
}

ska::Point<int> ClashArena::screenSize() const {
	return m_camera.getScreenSize();
}

ska::Point<float> ClashArena::toScreen(ska::Point<float> world) const {
	const auto& display = m_camera.getDisplay();
	const auto zoom = m_camera.getZoom();
	return ska::Point<float>{ (world.x - static_cast<float>(display.x)) * zoom, (world.y - static_cast<float>(display.y)) * zoom };
}

void ClashArena::capture() const {
	const auto& turn = m_stage.turn;
	if (m_captured == turn.id) {
		return;
	}
	m_captured = turn.id;
	auto& world = m_worldState.getWorld();
	const auto blockSize = static_cast<float>(world.getBlockSize());
	const auto& profile = world.getCollisionProfile();
	const auto& tileset = m_worldState.getTileset();
	const auto block = [&](ska::EntityId entity, ska::Point<float> fallback) {
		if (!m_entities.hasComponent<ska::PositionComponent>(entity) || !m_entities.hasComponent<ska::HitboxComponent>(entity)) {
			return fallback;
		}
		const auto center = ska::PositionComponent::getCenterPosition(m_entities.getComponent<ska::PositionComponent>(entity), m_entities.getComponent<ska::HitboxComponent>(entity));
		return ska::Point<float>{ center.x / blockSize, center.y / blockSize };
	};
	const auto middle = ska::Point<float>{ turn.center.x / blockSize, turn.center.y / blockSize };
	const auto player = block(turn.player, middle);
	const auto opponent = block(turn.opponent, middle);
	m_clashX = static_cast<int>(middle.x);
	m_clashY = static_cast<int>(middle.y);
	m_centerX = static_cast<int>(player.x);
	m_centerY = static_cast<int>(player.y);
	// The camera is behind the Pokemon of the player, it looks at the opponent (the nearest of the four directions)
	const auto dx = opponent.x - player.x;
	const auto dy = opponent.y - player.y;
	if (std::abs(dx) > std::abs(dy)) {
		m_forward = ska::Point<int>{ dx > 0.F ? 1 : -1, 0 };
	} else {
		m_forward = ska::Point<int>{ 0, dy > 0.F ? 1 : -1 };
	}
	m_cells.clear();
	const auto layers = std::min<std::size_t>(3, profile.layers());
	for (auto y = m_centerY - Radius; y <= m_centerY + Radius; y++) {
		for (auto x = m_centerX - Radius; x <= m_centerX + Radius; x++) {
			auto cell = Cell{};
			cell.x = x;
			cell.y = y;
			if (x >= 0 && y >= 0 && static_cast<std::size_t>(x) < profile.getBlocksX() && static_cast<std::size_t>(y) < profile.getBlocksY()) {
				for (auto layer = std::size_t{ 0 }; layer < layers; layer++) {
					const auto* tile = profile.getBlock(layer, static_cast<std::size_t>(x), static_cast<std::size_t>(y));
					if (tile != nullptr) {
						cell.clips[layer] = tileset.getAnimation(tile->id).animation.getOffsetBase();
						cell.bases[layer] = tileset.getTile(tile->id).properties.base;
					}
				}
			}
			m_cells.push_back(cell);
		}
	}
	m_laidOut = ska::Point<int>{};
}
const ClashArena::Cell* ClashArena::cell(int x, int y) const {
	constexpr auto Side = 2 * Radius + 1;
	const auto cx = x - (m_centerX - Radius);
	const auto cy = y - (m_centerY - Radius);
	if (m_cells.empty() || cx < 0 || cy < 0 || cx >= Side || cy >= Side) {
		return nullptr;
	}
	return &m_cells[static_cast<std::size_t>(cy * Side + cx)];
}

const ClashArena::Cell* ClashArena::wrapped(int x, int y) const {
	// Beyond the blocks kept, the ground goes on as a mirror of them
	constexpr auto Side = 2 * Radius + 1;
	const auto mirror = [](int v) {
		const auto period = 2 * Side;
		v = ((v % period) + period) % period;
		return v < Side ? v : period - 1 - v;
	};
	return cell(m_centerX - Radius + mirror(x - (m_centerX - Radius)), m_centerY - Radius + mirror(y - (m_centerY - Radius)));
}

ClashArena::View ClashArena::view() const {
	const auto screen = screenSize();
	const auto w = static_cast<float>(screen.x);
	const auto h = static_cast<float>(screen.y);
	auto v = View{};
	v.width = w;
	v.horizon = h * 0.42F;
	// The Pokemon of the player at 0.69 of the height : its block, "PlayerDepth" from the camera
	v.focal = (h * 0.69F - v.horizon) * PlayerDepth / CameraHeight;
	const auto playerSize = v.focal / PlayerDepth;
	v.lateral = (w * 0.5F - w * 0.26F) / playerSize + std::sin(static_cast<float>(m_time) / 3300.F) * 0.7F;
	// The opponent, in front, on the right
	const auto opponentDepth = CameraHeight * v.focal / (h * 0.16F);
	v.opponentForward = opponentDepth - PlayerDepth;
	v.opponentLateral = (w * 0.5F - w * 0.26F) / playerSize + (w * 0.71F - w * 0.5F) / (v.focal / opponentDepth);
	return v;
}

ska::Point<float> ClashArena::project(const View& v, float forward, float lateral, float up, float* size) const {
	const auto z = std::max(0.2F, PlayerDepth + forward);
	const auto s = v.focal / z;
	if (size != nullptr) {
		*size = s;
	}
	return ska::Point<float>{ v.width / 2.F + (lateral - v.lateral) * s, v.horizon + (CameraHeight - up) * s };
}
void ClashArena::layout(int width, int height) const {
	if (m_laidOut.x == width && m_laidOut.y == height) {
		return;
	}
	m_laidOut = ska::Point<int>{ width, height };
	m_ground.clear();
	m_standing.clear();
	m_rows.clear();
	const auto v = view();
	const auto right = ska::Point<int>{ -m_forward.y, m_forward.x };
	const auto at = [&](int forward, int lateral) {
		return ska::Point<int>{ m_centerX + forward * m_forward.x + lateral * right.x, m_centerY + forward * m_forward.y + lateral * right.y };
	};
	// The ground where a tile stands (a wall, a cliff) : the flat ground of the block of the player, else any
	auto fallback = std::optional<ska::Rectangle>{};
	if (const auto* own = cell(m_centerX, m_centerY); own != nullptr && own->clips[0].has_value() && own->bases[0] < 0) {
		fallback = own->clips[0];
	}
	for (const auto& c : m_cells) {
		if (!fallback.has_value() && c.clips[0].has_value() && c.bases[0] < 0) {
			fallback = c.clips[0];
		}
	}
	const auto shadeOf = [](float forward) { return Byte(255.F - Clamp01(forward / static_cast<float>(FarRow)) * 135.F); };
	const auto delayOf = [](float forward) { return 60.F + (1.F - Clamp01(forward / static_cast<float>(FarRow))) * 420.F; };

	// The ground : its flat tiles, row by row from the farthest
	for (auto forward = FarRow; forward >= NearRow; forward--) {
		if (fallback.has_value()) {
			auto row = Piece{};
			row.clip = *fallback;
			row.forward = static_cast<float>(forward);
			row.floor = true;
			row.shade = shadeOf(static_cast<float>(forward));
			row.delay = delayOf(static_cast<float>(forward));
			m_rows.push_back(row);
		}
		for (auto lateral = -SideBlocks; lateral <= SideBlocks; lateral++) {
			const auto block = at(forward, lateral);
			const auto* c = wrapped(block.x, block.y);
			if (c == nullptr) {
				continue;
			}
			auto piece = Piece{};
			piece.forward = static_cast<float>(forward);
			piece.lateral = static_cast<float>(lateral);
			piece.floor = true;
			piece.shade = shadeOf(piece.forward);
			piece.seed = Hash(block.x, block.y, 7);
			piece.delay = delayOf(piece.forward) + piece.seed * 60.F;
			const auto ground = c->clips[0].has_value() && c->bases[0] < 0 ? c->clips[0] : fallback;
			if (ground.has_value()) {
				piece.clip = *ground;
				m_ground.push_back(piece);
			}
			// The flat tiles over it (a carpet, a path)
			for (auto layer = 1; layer < 3; layer++) {
				const auto index = static_cast<std::size_t>(layer);
				if (c->clips[index].has_value() && c->bases[index] < 0) {
					piece.clip = *c->clips[index];
					m_ground.push_back(piece);
				}
			}
		}
	}

	// The tiles that stand (their depth : the block of their object on the ground, how high they are) : upright, as the
	// trees and the signs of the racing games, scaled by their distance
	// The ground around each tile that stands (its neighbour), to cut it out of the tile
	auto grounds = std::map<std::pair<int, int>, std::vector<ska::Rectangle>>{};
	const auto groundsNear = [&](const Cell& c, std::vector<ska::Rectangle>& found) {
		for (auto dy = -2; dy <= 2; dy++) {
			for (auto dx = -2; dx <= 2; dx++) {
				const auto* n = cell(c.x + dx, c.y + dy);
				if (n != nullptr && n->clips[0].has_value() && n->bases[0] < 0) {
					const auto& clip = *n->clips[0];
					if (std::none_of(found.begin(), found.end(), [&clip](const ska::Rectangle& r) { return r.x == clip.x && r.y == clip.y; })) {
						found.push_back(clip);
					}
				}
			}
		}
	};
	// The objects : the tiles that stand, side by side, on the same row of the ground (a tree, a house, a row of trees).
	// An object is drawn whole or not at all : not when it goes beyond the blocks kept, behind the camera or too far
	struct Node { const Cell* cell; int layer; int baseY; };
	auto nodes = std::vector<Node>{};
	auto byCell = std::map<std::pair<int, int>, std::vector<std::size_t>>{};
	for (const auto& c : m_cells) {
		for (auto layer = 0; layer < 3; layer++) {
			const auto index = static_cast<std::size_t>(layer);
			if (c.clips[index].has_value() && c.bases[index] >= 0) {
				byCell[{ c.x, c.y }].push_back(nodes.size());
				nodes.push_back(Node{ &c, layer, c.y + c.bases[index] });
			}
		}
	}
	auto parent = std::vector<std::size_t>(nodes.size());
	for (auto i = std::size_t{ 0 }; i < parent.size(); i++) {
		parent[i] = i;
	}
	const auto root = [&parent](std::size_t i) {
		while (parent[i] != i) {
			parent[i] = parent[parent[i]];
			i = parent[i];
		}
		return i;
	};
	for (auto i = std::size_t{ 0 }; i < nodes.size(); i++) {
		const int around[][2] = { { 0, 0 }, { 1, 0 }, { 0, 1 } };
		for (const auto& d : around) {
			const auto found = byCell.find({ nodes[i].cell->x + d[0], nodes[i].cell->y + d[1] });
			if (found == byCell.end()) {
				continue;
			}
			for (const auto j : found->second) {
				if (j != i && nodes[j].baseY == nodes[i].baseY) {
					parent[root(j)] = root(i);
				}
			}
		}
	}
	auto dropped = std::vector<char>(nodes.size(), 0);
	for (auto i = std::size_t{ 0 }; i < nodes.size(); i++) {
		const auto& n = nodes[i];
		const auto rx = n.cell->x - m_centerX;
		const auto ry = n.baseY - m_centerY;
		const auto forward = static_cast<float>(rx * m_forward.x + ry * m_forward.y);
		const auto border = std::abs(n.cell->x - m_centerX) >= Radius - 1 || std::abs(n.cell->y - m_centerY) >= Radius - 1;
		if (border || forward < -0.5F || forward > static_cast<float>(FarRow)) {
			dropped[root(i)] = 1;
		}
	}
	for (auto i = std::size_t{ 0 }; i < nodes.size(); i++) {
		if (dropped[root(i)] != 0) {
			continue;
		}
		const auto& c = *nodes[i].cell;
		const auto layer = nodes[i].layer;
		const auto index = static_cast<std::size_t>(layer);
		const auto rx = c.x - m_centerX;
		const auto ry = nodes[i].baseY - m_centerY;
		const auto forward = static_cast<float>(rx * m_forward.x + ry * m_forward.y);
		const auto lateral = static_cast<float>(rx * right.x + ry * right.y);
		auto piece = Piece{};
		piece.clip = *c.clips[index];
		const auto key = std::make_pair(piece.clip.x, piece.clip.y);
		// The two lowest rows of an object (a trunk and the foliage over it, the base of a wall) : drawn over the ground
		if (c.bases[index] <= 1) {
			groundsNear(c, grounds[key]);
		}
		piece.forward = forward;
		piece.lateral = lateral;
		piece.up = static_cast<float>(c.bases[index]) + 0.5F;
		piece.shade = shadeOf(forward);
		// The tiles of an object come together
		piece.seed = Hash(static_cast<int>(root(i)), 3);
		piece.delay = 280.F + (1.F - Clamp01(forward / static_cast<float>(FarRow))) * 360.F + piece.seed * 120.F;
		m_standing.push_back(piece);
	}
	// The farthest first, the lowest first
	std::stable_sort(m_standing.begin(), m_standing.end(), [](const Piece& a, const Piece& b) {
		return a.forward != b.forward ? a.forward > b.forward : a.up < b.up;
	});
	cutGround(grounds);
}

void ClashArena::cutGround(const std::map<std::pair<int, int>, std::vector<ska::Rectangle>>& grounds) const {
	m_hasStandingTiles = false;
	// The transparent colour of the tilesets (the colour key of the textures)
	constexpr std::uint8_t Key[] = { ska::Texture::DEFAULT_T_RED, ska::Texture::DEFAULT_T_GREEN, ska::Texture::DEFAULT_T_BLUE };
	try {
		auto image = worldgen::ReadPng(m_worldState.getTileset().getName() + ".png");
		const auto isKey = [&](const std::uint8_t* p) { return p[0] == Key[0] && p[1] == Key[1] && p[2] == Key[2]; };
		for (const auto& [at, near] : grounds) {
			if (near.empty()) {
				continue;
			}
			const auto size = std::min(near.front().w, near.front().h);
			if (at.first + size > image.width || at.second + size > image.height) {
				continue;
			}
			// The colours of the grounds around it
			auto palette = std::vector<std::array<int, 3>>{};
			for (const auto& ground : near) {
				if (ground.x + size > image.width || ground.y + size > image.height) {
					continue;
				}
				for (auto j = 0; j < size; j++) {
					for (auto i = 0; i < size; i++) {
						const auto* g = image.at(ground.x + i, ground.y + j);
						if (isKey(g)) {
							continue;
						}
						const auto colour = std::array<int, 3>{ g[0], g[1], g[2] };
						if (std::find(palette.begin(), palette.end(), colour) == palette.end()) {
							palette.push_back(colour);
						}
					}
				}
			}
			// A pixel of the ground, or of its shadow (a colour of the ground, darker)
			const auto groundLike = [&](const std::uint8_t* t) {
				for (const auto& c : palette) {
					if (std::abs(t[0] - c[0]) + std::abs(t[1] - c[1]) + std::abs(t[2] - c[2]) <= 9) {
						return true;
					}
					if (c[0] > 12 && c[1] > 12 && c[2] > 12) {
						const auto r = static_cast<float>(t[0]) / static_cast<float>(c[0]);
						const auto g = static_cast<float>(t[1]) / static_cast<float>(c[1]);
						const auto b = static_cast<float>(t[2]) / static_cast<float>(c[2]);
						if (r > 0.35F && r < 0.97F && std::abs(r - g) < 0.05F && std::abs(r - b) < 0.05F) {
							return true;
						}
					}
				}
				return false;
			};
			// From the border of the tile inwards : the ground around the object (not the pixels inside it of the same colour)
			auto cut = std::vector<char>(static_cast<std::size_t>(size * size), 0);
			auto stack = std::vector<std::pair<int, int>>{};
			for (auto k = 0; k < size; k++) {
				stack.emplace_back(k, 0);
				stack.emplace_back(k, size - 1);
				stack.emplace_back(0, k);
				stack.emplace_back(size - 1, k);
			}
			while (!stack.empty()) {
				const auto [i, j] = stack.back();
				stack.pop_back();
				if (i < 0 || j < 0 || i >= size || j >= size) {
					continue;
				}
				auto& done = cut[static_cast<std::size_t>(j * size + i)];
				if (done != 0) {
					continue;
				}
				auto* t = image.at(at.first + i, at.second + j);
				if (!isKey(t) && !groundLike(t)) {
					continue;
				}
				done = 1;
				t[0] = Key[0];
				t[1] = Key[1];
				t[2] = Key[2];
				stack.emplace_back(i + 1, j);
				stack.emplace_back(i - 1, j);
				stack.emplace_back(i, j + 1);
				stack.emplace_back(i, j - 1);
			}
		}
		// A new file each time (the textures are kept by their file name)
		static auto made = 0U;
		const auto path = (std::filesystem::temp_directory_path() / ("sad_clash_tiles_" + std::to_string(++made) + ".bmp")).string();
		worldgen::WriteFile(path, worldgen::EncodeBmp(image));
		m_standingTiles = ska::Texture{ path };
		m_hasStandingTiles = m_standingTiles.getWidth() > 0;
		std::remove(path.c_str());
	} catch (const std::exception& e) {
		SLOG(ska::LogLevel::Warn) << "Clash : the tiles that stand keep their ground (" << e.what() << ")";
	}
}
ska::Point<int> ClashArena::shake() const {
	const auto& turn = m_stage.turn;
	auto amplitude = 0.F;
	const auto t = static_cast<float>(turn.time);
	const auto decay = [](float since, float duration, float strength) { return since >= 0.F && since < duration ? strength * (1.F - since / duration) : 0.F; };
	switch (turn.step) {
	case Step::Versus:
		amplitude = decay(t - 560.F, 320.F, 14.F);
		break;
	case Step::Rebuild:
		amplitude = std::max(decay(t - 700.F, 220.F, 9.F), decay(t - 860.F, 220.F, 7.F));
		break;
	case Step::Round:
		amplitude = decay(t - static_cast<float>(BattleTurn::FightAt), 320.F, 12.F);
		break;
	case Step::Acting:
		if (turn.hit && turn.damage > 0) {
			amplitude = decay(static_cast<float>(turn.hitAge), 380.F, 16.F);
		}
		break;
	case Step::KnockOut:
		amplitude = decay(t, 600.F, 22.F);
		break;
	default:
		break;
	}
	const auto time = static_cast<float>(m_time);
	return ska::Point<int>{ static_cast<int>(std::sin(time * 0.11F) * amplitude), static_cast<int>(std::cos(time * 0.137F) * amplitude * 0.7F) };
}

const ska::Texture& ClashArena::charset(int id) const {
	auto found = m_charsets.find(id);
	if (found == m_charsets.end()) {
		found = m_charsets.emplace(id, ska::Texture{ ska::SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, id) }).first;
	}
	return found->second;
}

int ClashArena::monster(ska::EntityId entity) const {
	return m_entities.hasComponent<BattleComponent>(entity) ? m_entities.getComponent<BattleComponent>(entity).monster : 0;
}

void ClashArena::tile(ska::Renderer& renderer, const ska::Rectangle& clip, float x, float y, float width, float height, double angle, unsigned char alpha, unsigned char shade, unsigned char flash, bool standing) const {
	if (alpha == 0 || clip.w <= 0 || clip.h <= 0) {
		return;
	}
	const auto& texture = standing && m_hasStandingTiles ? m_standingTiles : m_worldState.getTileset().getTexture();
	auto effect = ska::RenderEffect{};
	effect.scaleX = width / static_cast<float>(clip.w);
	effect.scaleY = height / static_cast<float>(clip.h);
	effect.anchorBottom = false;
	effect.alpha = alpha;
	effect.red = shade;
	effect.green = shade;
	effect.blue = shade;
	effect.flash = flash;
	renderer.setEffect(&effect);
	renderer.render(texture, static_cast<int>(x) - clip.w / 2, static_cast<int>(y) - clip.h / 2, &clip, angle, nullptr);
	renderer.setEffect(nullptr);
}

void ClashArena::pokemon(ska::Renderer& renderer, int id, int row, int column, float x, float y, float scale, unsigned char alpha, unsigned char flash, int sink, unsigned char shade) const {
	const auto& texture = charset(id);
	const auto frame = static_cast<int>(texture.getHeight()) / 8;
	if (frame <= 0 || alpha == 0) {
		return;
	}
	auto clip = ska::Rectangle{ column * frame, row * frame, frame, std::max(1, frame - sink) };
	auto effect = ska::RenderEffect{};
	effect.scaleX = scale;
	effect.scaleY = scale;
	effect.anchorBottom = true;
	effect.alpha = alpha;
	effect.flash = flash;
	effect.red = shade;
	effect.green = shade;
	effect.blue = shade;
	renderer.setEffect(&effect);
	renderer.render(texture, static_cast<int>(x) - frame / 2, static_cast<int>(y) - clip.h, &clip, 0., nullptr);
	renderer.setEffect(nullptr);
}

ClashArena::Fighter ClashArena::fighter(ska::EntityId entity) const {
	const auto screen = screenSize();
	const auto h = static_cast<float>(screen.y);
	const auto v = view();
	auto result = Fighter{};
	result.entity = entity;
	if (entity == m_stage.turn.player) {
		// From behind, close
		result.feet = project(v, 0.F, 0.F, 0.F);
		result.scale = h / 140.F;
		result.row = 0;
		result.forward = 0.F;
	} else {
		// In front, farther
		result.feet = project(v, v.opponentForward, v.opponentLateral, 0.F);
		result.scale = h / 215.F;
		result.row = 2;
		result.forward = v.opponentForward;
	}
	return result;
}
void ClashArena::render(ska::Renderer& renderer) const {
	const auto& turn = m_stage.turn;
	if (!turn.active()) {
		return;
	}
	capture();
	const auto screen = screenSize();
	layout(screen.x, screen.y);
	auto canvas = Ui::Canvas{ renderer };
	const auto t = static_cast<float>(turn.time);
	const auto all = ska::Rectangle{ 0, 0, screen.x, screen.y };

	switch (turn.step) {
	case Step::Impact:
	case Step::Zoom:
		renderImpact(canvas);
		break;
	case Step::Shatter:
		renderShatter(canvas);
		break;
	case Step::Versus:
		renderVersus(canvas);
		break;
	case Step::Rebuild:
		renderScene(canvas, t, -1.F);
		renderHud(canvas, Clamp01((t - 900.F) / 200.F));
		// The flash of the end of the screen "VS"
		canvas.fill(Rgba(255, 255, 255, 255.F * (1.F - Clamp01(t / 260.F))), all);
		break;
	case Step::Round:
		renderScene(canvas, 1e6F, -1.F);
		renderHud(canvas, 1.F);
		renderRound(canvas);
		break;
	case Step::Menu:
		renderScene(canvas, 1e6F, -1.F);
		renderHud(canvas, 1.F);
		renderMenu(canvas);
		break;
	case Step::Acting:
	case Step::RoundEnd:
		renderScene(canvas, 1e6F, -1.F);
		renderHud(canvas, 1.F);
		renderMessage(canvas, 1.F);
		break;
	case Step::KnockOut:
		renderScene(canvas, 1e6F, -1.F);
		renderHud(canvas, 1.F);
		renderKnockOut(canvas);
		break;
	case Step::Return:
		renderScene(canvas, 1e6F, t);
		renderHud(canvas, 1.F - Clamp01(t / 250.F));
		canvas.fill(Rgba(255, 255, 255, 230.F * (1.F - Clamp01(t / 160.F))), all);
		break;
	}
}

void ClashArena::renderImpact(Ui::Canvas& canvas) const {
	const auto& turn = m_stage.turn;
	const auto screen = screenSize();
	const auto w = static_cast<float>(screen.x);
	const auto h = static_cast<float>(screen.y);
	const auto total = static_cast<float>(turn.total);
	const auto zoom = turn.step == Step::Zoom ? static_cast<float>(turn.time) / static_cast<float>(BattleTurn::ZoomDuration) : 0.F;
	const auto focus = toScreen(turn.center);
	const auto frame = static_cast<int>(m_time / 45);
	const auto opening = turn.kind == BattleTurn::Kind::Opening;

	// The vignette closes, black bars (the zoom)
	const auto vignette = Clamp01(total / 300.F);
	for (auto i = 0; i < 12; i++) {
		const auto band = w * 0.014F;
		const auto inset = static_cast<int>(static_cast<float>(i) * band);
		const auto color = Rgba(0, 0, 0, (16.F + 10.F * zoom) * vignette);
		const auto thick = static_cast<int>(band) + 1;
		canvas.fill(color, ska::Rectangle{ 0, inset, screen.x, thick });
		canvas.fill(color, ska::Rectangle{ 0, screen.y - inset - thick, screen.x, thick });
		canvas.fill(color, ska::Rectangle{ inset, 0, thick, screen.y });
		canvas.fill(color, ska::Rectangle{ screen.x - inset - thick, 0, thick, screen.y });
	}

	// The focus lines of the mangas, on the point of the clash
	const auto linesAlpha = 170.F * Clamp01(total / 60.F);
	for (auto i = 0; i < 80; i++) {
		const auto angle = static_cast<float>(i) * 2.F * Pi / 80.F + Hash(i, 3) * 0.06F;
		const auto inner = (0.30F + 0.16F * Hash(i, frame)) * w * 0.5F * (1.F - 0.35F * zoom);
		const auto dx = std::cos(angle);
		const auto dy = std::sin(angle);
		Line(canvas, Rgba(255, 255, 255, linesAlpha * (0.5F + 0.5F * Hash(i, frame, 1))), focus.x + dx * inner, focus.y + dy * inner, focus.x + dx * w * 1.5F, focus.y + dy * w * 1.5F, 1 + static_cast<int>(Hash(i, 9) * 3.F));
	}

	// The energies of the two Pokemon meet : bolts, a shock wave, sparks
	if (total < 520.F) {
		const auto reach = EaseOut(total / 120.F) * w * 0.22F;
		const auto fade = 255.F * (1.F - Clamp01((total - 300.F) / 220.F));
		for (auto i = 0; i < 8; i++) {
			const auto angle = static_cast<float>(i) * 2.F * Pi / 8.F + Hash(i, frame / 2) * 0.5F;
			const auto glow = i % 2 == 0 ? Rgba(80, 170, 255) : Rgba(255, 80, 60);
			Bolt(canvas, focus.x, focus.y, focus.x + std::cos(angle) * reach, focus.y + std::sin(angle) * reach, 18.F, i * 31 + frame, glow, fade);
		}
	}
	if (total < 600.F) {
		const auto radius = total * 1.5F;
		const auto alpha = 230.F * (1.F - total / 600.F);
		for (auto i = 0; i < 72; i++) {
			const auto angle = static_cast<float>(i) * 2.F * Pi / 72.F;
			canvas.fill(Rgba(255, 240, 200, alpha), ska::Rectangle{ static_cast<int>(focus.x + std::cos(angle) * radius) - 3, static_cast<int>(focus.y + std::sin(angle) * radius * 0.8F) - 3, 6, 6 });
		}
	}
	if (total < 800.F) {
		for (auto i = 0; i < 40; i++) {
			const auto angle = Hash(i, 11) * 2.F * Pi;
			const auto speed = 0.45F + Hash(i, 12) * 0.9F;
			const auto x = focus.x + std::cos(angle) * speed * total;
			const auto y = focus.y + std::sin(angle) * speed * total + 0.0011F * total * total;
			const auto size = static_cast<int>(8.F * (1.F - total / 800.F)) + 1;
			const auto color = i % 3 == 0 ? Rgba(255, 255, 255) : Rgba(255, 220, 80);
			canvas.fill(color, ska::Rectangle{ static_cast<int>(x), static_cast<int>(y), size, size });
		}
	}

	// A heartbeat (the zoom), black bars
	if (turn.step == Step::Zoom) {
		const auto beat = [&](float at) { const auto d = static_cast<float>(turn.time) - at; return d >= 0.F && d < 160.F ? 1.F - d / 160.F : 0.F; };
		const auto pulse = std::max(beat(0.F), beat(280.F));
		canvas.fill(Rgba(200, 20, 30, 80.F * pulse), ska::Rectangle{ 0, 0, screen.x, screen.y });
	}
	const auto bars = static_cast<int>(EaseOut((total - 250.F) / 350.F) * h * 0.09F);
	canvas.fill(Rgba(0, 0, 0), ska::Rectangle{ 0, 0, screen.x, bars });
	canvas.fill(Rgba(0, 0, 0), ska::Rectangle{ 0, screen.y - bars, screen.x, bars });

	// "CLASH !" stamped, then it goes up
	if (total >= 60.F) {
		const auto stamp = 1.F + 2.6F * (1.F - EaseOut((total - 60.F) / 150.F));
		const auto y = h * 0.2F - EaseIn(zoom) * h * 0.3F;
		const auto alpha = 255.F * Clamp01((total - 60.F) / 60.F) * (1.F - Clamp01((zoom - 0.6F) / 0.4F));
		const auto wiggle = total < 400.F ? std::sin(total * 0.08F) * 6.F * (1.F - total / 400.F) : 0.F;
		Shout(canvas, opening ? "OUVERTURE !" : "CLASH !", 72, opening ? Rgba(110, 235, 255) : Rgba(255, 220, 60), opening ? Rgba(10, 40, 100) : Rgba(140, 20, 10), w / 2.F + wiggle, y, stamp, alpha);
	}

	// The flash of the impact
	canvas.fill(Rgba(255, 255, 255, 255.F * (1.F - Clamp01(total / 130.F))), ska::Rectangle{ 0, 0, screen.x, screen.y });
}

void ClashArena::renderShatter(Ui::Canvas& canvas) const {
	const auto& turn = m_stage.turn;
	const auto screen = screenSize();
	const auto w = static_cast<float>(screen.x);
	const auto h = static_cast<float>(screen.y);
	const auto t = static_cast<float>(turn.time);
	auto& renderer = canvas.renderer();
	const auto blockSize = static_cast<float>(m_worldState.getWorld().getBlockSize());
	const auto zoom = m_camera.getZoom();
	const auto size = blockSize * zoom;
	const auto night = [h](float y) {
		const auto u = Clamp01(y / h);
		return Rgba(static_cast<int>(Lerp(14.F, 52.F, u)), static_cast<int>(Lerp(8.F, 18.F, u)), static_cast<int>(Lerp(34.F, 64.F, u)));
	};

	// The blocks break away from the middle outwards : the night behind them
	struct Flying { const Cell* cell; float x; float y; float progress; };
	auto flying = std::vector<Flying>{};
	for (const auto& c : m_cells) {
		const auto at = toScreen(ska::Point<float>{ static_cast<float>(c.x) * blockSize, static_cast<float>(c.y) * blockSize });
		if (at.x + size < 0.F || at.y + size < 0.F || at.x > w || at.y > h) {
			continue;
		}
		const auto dx = static_cast<float>(c.x - m_clashX);
		const auto dy = static_cast<float>(c.y - m_clashY);
		const auto departure = std::sqrt(dx * dx + dy * dy) / ShatterRadius * 560.F + Hash(c.x, c.y) * 90.F;
		if (t < departure) {
			continue;
		}
		for (auto y = 0; y < static_cast<int>(size) + 1; y += 4) {
			canvas.fill(night(at.y + static_cast<float>(y)), ska::Rectangle{ static_cast<int>(at.x), static_cast<int>(at.y) + y, static_cast<int>(size) + 1, 4 });
		}
		const auto progress = (t - departure) / 520.F;
		if (progress < 1.F) {
			flying.push_back(Flying{ &c, at.x + size / 2.F, at.y + size / 2.F, progress });
		}
	}
	// Everything else goes dark too
	const auto dark = Clamp01((t - 520.F) / 300.F);
	if (dark > 0.F) {
		for (auto y = 0; y < screen.y; y += 6) {
			auto color = night(static_cast<float>(y));
			color.a = Byte(255.F * dark);
			canvas.fill(color, ska::Rectangle{ 0, y, screen.x, 6 });
		}
	}
	for (const auto& f : flying) {
		const auto seed = Hash(f.cell->x, f.cell->y, 5);
		const auto spin = (seed - 0.5F) * 2.F;
		const auto p = f.progress;
		const auto x = f.x + spin * 90.F * p;
		const auto y = f.y - 110.F * p + 900.F * p * p;
		const auto scale = 1.F - 0.4F * p;
		const auto alpha = Byte(255.F * (1.F - p * p));
		const auto flash = Byte(p < 0.12F ? 200.F * (1.F - p / 0.12F) : 0.F);
		for (const auto& clip : f.cell->clips) {
			if (clip.has_value()) {
				tile(renderer, *clip, x, y, size * scale, size * scale, static_cast<double>(spin * 320.F * p), alpha, 255, flash);
			}
		}
		// Dust
		if (p < 0.5F) {
			for (auto i = 0; i < 3; i++) {
				const auto dx = (Hash(f.cell->x, f.cell->y, 20 + i) - 0.5F) * size;
				canvas.fill(Rgba(210, 190, 160, 200.F * (1.F - p * 2.F)), ska::Rectangle{ static_cast<int>(f.x + dx), static_cast<int>(f.y + size * 0.4F + p * 40.F), 5, 5 });
			}
		}
	}

	// The two Pokemon stay, lit (as the map draws them)
	for (const auto entity : { turn.player, turn.opponent }) {
		if (!m_entities.hasComponent<ska::GraphicComponent>(entity) || !m_entities.hasComponent<ska::PositionComponent>(entity) || !m_entities.hasComponent<ska::HitboxComponent>(entity)) {
			continue;
		}
		auto& graphic = m_entities.getComponent<ska::GraphicComponent>(entity);
		if (graphic.animatedSprites.empty()) {
			continue;
		}
		const auto& hitbox = m_entities.getComponent<ska::HitboxComponent>(entity);
		const auto drawn = graphic.drawnPosition(m_entities.getComponent<ska::PositionComponent>(entity));
		const auto origin = graphic.spriteOrigin(std::lround(drawn.x), std::lround(drawn.y), hitbox);
		const auto& sprite = graphic.animatedSprites[0];
		const auto sw = static_cast<float>(sprite.getWidth());
		const auto sh = static_cast<float>(sprite.getHeight());
		const auto top = toScreen(ska::Point<float>{ static_cast<float>(origin.x), static_cast<float>(origin.y) - static_cast<float>(drawn.z) });
		const auto feetX = top.x + sw * zoom / 2.F;
		const auto feetY = top.y + sh * zoom;
		const auto glow = entity == turn.player ? Rgba(80, 160, 255, 90.F) : Rgba(255, 80, 60, 90.F);
		Ellipse(canvas, feetX, feetY - 4.F, sw * zoom * 0.45F, sw * zoom * 0.14F, glow);
		auto effect = ska::RenderEffect{};
		effect.scaleX = zoom;
		effect.scaleY = zoom;
		effect.anchorBottom = true;
		effect.flash = Byte(60.F + 50.F * std::sin(static_cast<float>(m_time) / 70.F));
		renderer.setEffect(&effect);
		sprite.render(renderer, static_cast<int>(feetX - sw / 2.F), static_cast<int>(feetY - sh), nullptr, 0., nullptr);
		renderer.setEffect(nullptr);
	}

	// The black bars stay
	const auto bars = static_cast<int>(h * 0.09F);
	canvas.fill(Rgba(0, 0, 0), ska::Rectangle{ 0, 0, screen.x, bars });
	canvas.fill(Rgba(0, 0, 0), ska::Rectangle{ 0, screen.y - bars, screen.x, bars });
	// The light grows before the screen "VS"
	canvas.fill(Rgba(255, 255, 255, 255.F * EaseIn((t - 700.F) / 200.F)), ska::Rectangle{ 0, 0, screen.x, screen.y });
}

void ClashArena::renderVersus(Ui::Canvas& canvas) const {
	const auto& turn = m_stage.turn;
	const auto screen = screenSize();
	const auto w = static_cast<float>(screen.x);
	const auto h = static_cast<float>(screen.y);
	const auto t = static_cast<float>(turn.time);
	auto& renderer = canvas.renderer();
	const auto offset = shake();
	const auto ox = static_cast<float>(offset.x);
	const auto oy = static_cast<float>(offset.y);

	canvas.fill(Rgba(10, 6, 20), ska::Rectangle{ 0, 0, screen.x, screen.y });
	// The two sides slide in, split by a diagonal
	const auto slide = 1.F - EaseOut(t / 260.F);
	for (auto y = 0; y < screen.y; y += 4) {
		const auto u = static_cast<float>(y) / h;
		const auto split = Lerp(w * 0.60F, w * 0.40F, u) + ox;
		const auto left = -slide * w;
		const auto right = slide * w;
		canvas.fill(Rgba(static_cast<int>(20.F + 40.F * u), static_cast<int>(60.F + 30.F * u), static_cast<int>(200.F - 60.F * u)), ska::Rectangle{ static_cast<int>(left), y, static_cast<int>(split - left), 4 });
		canvas.fill(Rgba(static_cast<int>(150.F + 80.F * u), 30, static_cast<int>(50.F - 20.F * u)), ska::Rectangle{ static_cast<int>(split + right), y, static_cast<int>(w - split + 4.F), 4 });
	}
	// Speed lines in each side
	for (auto i = 0; i < 16; i++) {
		const auto y = Hash(i, 40) * h;
		const auto speed = 1.2F + Hash(i, 41) * 1.6F;
		const auto run = std::fmod(static_cast<float>(m_time) * speed + Hash(i, 42) * w, w + 300.F) - 150.F;
		const auto length = 80.F + Hash(i, 43) * 160.F;
		if (y > h * 0.5F) {
			Line(canvas, Rgba(200, 230, 255, 70.F), run, y, run + length, y, 2);
		} else {
			Line(canvas, Rgba(255, 220, 200, 70.F), w - run, y, w - run - length, y, 2);
		}
	}
	// The lightning between the two sides
	if (t > 200.F) {
		Bolt(canvas, w * 0.60F + ox, -10.F, w * 0.40F + ox, h + 10.F, 22.F, static_cast<int>(m_time / 50), Rgba(255, 220, 90), 255.F * Clamp01((t - 200.F) / 100.F));
	}

	// The two Pokemon, big, their silhouettes then a flash (each faces the other)
	const auto entrance = [&](float delay) { return EaseOutBack((t - delay) / 320.F); };
	const auto reveal = [&](float at) {
		if (t < at) { return std::pair<unsigned char, unsigned char>{ 0, 0 }; }
		return std::pair<unsigned char, unsigned char>{ 255, Byte(255.F * (1.F - (t - at) / 160.F)) };
	};
	const auto scale = h / 95.F;
	const auto bob = std::sin(static_cast<float>(m_time) / 240.F) * 4.F;
	const auto column = static_cast<int>(m_time / 180) % 3;
	{
		const auto [shade, flash] = reveal(440.F);
		const auto x = w * 0.24F - (1.F - entrance(80.F)) * w * 0.6F + ox;
		pokemon(renderer, monster(turn.player), 1, column, x, h * 0.84F + bob + oy, scale, 255, flash, 0, shade);
	}
	{
		const auto [shade, flash] = reveal(520.F);
		const auto x = w * 0.76F + (1.F - entrance(160.F)) * w * 0.6F + ox;
		pokemon(renderer, monster(turn.opponent), 3, column, x, h * 0.84F - bob + oy, scale, 255, flash, 0, shade);
	}

	// Their names
	const auto level = [this](ska::EntityId entity) { return m_entities.hasComponent<BattleComponent>(entity) ? m_entities.getComponent<BattleComponent>(entity).level : 1; };
	const auto names = EaseOut((t - 300.F) / 300.F);
	canvas.text(Upper(turn.pokemon), Style(54, Rgba(255, 255, 255), 4, Rgba(10, 30, 110), 1.F, 255.F * names), static_cast<int>(w * 0.05F - (1.F - names) * 300.F), static_cast<int>(h * 0.04F), Ui::Align::Left);
	canvas.text("N." + std::to_string(level(turn.player)), Style(26, Rgba(190, 220, 255), 3, Rgba(10, 30, 110), 1.F, 255.F * names), static_cast<int>(w * 0.05F - (1.F - names) * 300.F), static_cast<int>(h * 0.04F) + 64, Ui::Align::Left);
	canvas.text(Upper(turn.opponentName), Style(54, Rgba(255, 255, 255), 4, Rgba(110, 10, 10), 1.F, 255.F * names), static_cast<int>(w * 0.95F + (1.F - names) * 300.F), static_cast<int>(h * 0.80F), Ui::Align::Right);
	canvas.text("N." + std::to_string(level(turn.opponent)), Style(26, Rgba(255, 210, 200), 3, Rgba(110, 10, 10), 1.F, 255.F * names), static_cast<int>(w * 0.95F + (1.F - names) * 300.F), static_cast<int>(h * 0.80F) - 34, Ui::Align::Right);

	// "VS" lands
	if (t >= 380.F) {
		const auto stamp = 1.F + 4.F * (1.F - EaseOutBack((t - 380.F) / 200.F));
		const auto pulse = 1.F + 0.05F * std::sin(static_cast<float>(m_time) / 90.F);
		Shout(canvas, "VS", 96, Rgba(255, 210, 50), Rgba(130, 20, 10), w / 2.F + ox, h * 0.45F + oy, 1.6F * stamp * pulse, 255.F);
	}
	// The flash out
	canvas.fill(Rgba(255, 255, 255, 255.F * EaseIn((t - 1250.F) / 250.F)), ska::Rectangle{ 0, 0, screen.x, screen.y });
}

void ClashArena::renderSky(Ui::Canvas& canvas, float alpha, ska::Point<int> offset) const {
	const auto screen = screenSize();
	const auto w = static_cast<float>(screen.x);
	const auto h = static_cast<float>(screen.y);
	const auto horizon = view().horizon + static_cast<float>(offset.y);
	// A sunset of the fighting games : violet, pink, orange
	for (auto y = 0; y < static_cast<int>(horizon) + 4; y += 4) {
		const auto u = Clamp01(static_cast<float>(y) / horizon);
		const auto r = u < 0.6F ? Lerp(30.F, 170.F, u / 0.6F) : Lerp(170.F, 255.F, (u - 0.6F) / 0.4F);
		const auto g = u < 0.6F ? Lerp(18.F, 50.F, u / 0.6F) : Lerp(50.F, 160.F, (u - 0.6F) / 0.4F);
		const auto b = u < 0.6F ? Lerp(80.F, 120.F, u / 0.6F) : Lerp(120.F, 80.F, (u - 0.6F) / 0.4F);
		canvas.fill(Rgba(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), 255.F * alpha), ska::Rectangle{ 0, y, screen.x, 4 });
	}
	// Stars that twinkle
	for (auto i = 0; i < 46; i++) {
		const auto x = Hash(i, 60) * w;
		const auto y = Hash(i, 61) * horizon * 0.5F;
		const auto twinkle = 0.5F + 0.5F * std::sin(static_cast<float>(m_time) / (180.F + Hash(i, 62) * 300.F) + static_cast<float>(i));
		const auto size = 1 + static_cast<int>(Hash(i, 63) * 2.F);
		canvas.fill(Rgba(255, 255, 230, 220.F * twinkle * alpha), ska::Rectangle{ static_cast<int>(x) + offset.x / 3, static_cast<int>(y), size, size });
	}
	// The sun, cut by stripes
	const auto radius = h * 0.13F;
	const auto sunX = w * 0.63F + static_cast<float>(offset.x) / 2.F;
	const auto sunY = horizon - h * 0.07F;
	for (auto dy = -radius; dy <= radius; dy += 2.F) {
		const auto v = (dy + radius) / (2.F * radius);
		const auto y = sunY + dy;
		if (y > horizon) {
			break;
		}
		const auto stripe = v > 0.5F && static_cast<int>((v - 0.5F) * 30.F + static_cast<float>(m_time) / 400.F) % 2 == 0;
		if (stripe) {
			continue;
		}
		const auto half = radius * std::sqrt(std::max(0.F, 1.F - (dy / radius) * (dy / radius)));
		canvas.fill(Rgba(255, static_cast<int>(Lerp(240.F, 120.F, v)), static_cast<int>(Lerp(140.F, 70.F, v)), 255.F * alpha), ska::Rectangle{ static_cast<int>(sunX - half), static_cast<int>(y), static_cast<int>(2.F * half), 2 });
	}
	// Streaks in the wind
	for (auto i = 0; i < 7; i++) {
		const auto y = horizon * (0.15F + 0.7F * Hash(i, 70));
		const auto run = std::fmod(static_cast<float>(m_time) * (0.08F + 0.1F * Hash(i, 71)) + Hash(i, 72) * w, w + 400.F) - 200.F;
		canvas.fill(Rgba(255, 220, 240, 50.F * alpha), ska::Rectangle{ static_cast<int>(w - run), static_cast<int>(y), 180 + static_cast<int>(Hash(i, 73) * 200.F), 2 });
	}
}

void ClashArena::renderPieces(Ui::Canvas& canvas, const std::vector<Piece>& pieces, float build, float scatter, ska::Point<int> offset, float from, float to) const {
	const auto screen = screenSize();
	const auto w = static_cast<float>(screen.x);
	const auto h = static_cast<float>(screen.y);
	auto& renderer = canvas.renderer();
	const auto v = view();
	for (const auto& piece : pieces) {
		if (piece.forward < from || piece.forward >= to) {
			continue;
		}
		const auto p = (build - piece.delay) / 340.F;
		if (p <= 0.F) {
			continue;
		}
		auto size = 0.F;
		auto x = 0.F;
		auto y = 0.F;
		auto width = 0.F;
		auto height = 0.F;
		if (piece.floor) {
			// Its row : from the far edge of the block to its near edge
			const auto far = project(v, piece.forward + 0.5F, piece.lateral, 0.F, &size);
			const auto near = project(v, piece.forward - 0.5F, piece.lateral, 0.F);
			const auto centre = project(v, piece.forward, piece.lateral, 0.F, &size);
			x = centre.x;
			y = (far.y + near.y) / 2.F;
			width = size + 2.F;
			height = near.y - far.y + 2.F;
		} else {
			const auto centre = project(v, piece.forward, piece.lateral, piece.up, &size);
			x = centre.x;
			y = centre.y;
			// A pixel over its neighbours : no seam between the columns, whatever the rounding
			width = size + 2.F;
			height = size + 2.F;
		}
		x += static_cast<float>(offset.x);
		y += static_cast<float>(offset.y);
		auto angle = 0.F;
		auto alpha = Clamp01(p * 3.F);
		auto flash = 0.F;
		if (p < 1.F) {
			if (piece.floor) {
				// It flies in from where it broke
				const auto from = piece.seed * 2.F * Pi;
				const auto back = 1.F - EaseOutBack(p);
				x += std::cos(from) * w * 0.6F * back;
				y += (std::sin(from) * h * 0.5F - h * 0.3F) * back;
				angle = (piece.seed > 0.5F ? 1.F : -1.F) * 360.F * (1.F - EaseOut(p));
			} else {
				// It rises from its block on the ground
				const auto ground = project(v, piece.forward, piece.lateral, 0.F).y + static_cast<float>(offset.y);
				y = ground - (ground - y) * EaseOutBack(p);
				height *= std::max(0.05F, EaseOutBack(p));
			}
			flash = p > 0.85F ? 255.F * (1.F - (p - 0.85F) / 0.15F) * 0.6F : 0.F;
		}
		if (scatter >= 0.F) {
			// It breaks away
			const auto q = Clamp01((scatter - piece.seed * 220.F) / 520.F);
			const auto dx = x - w / 2.F;
			const auto dy = y - h * 0.55F;
			const auto length = std::max(1.F, std::sqrt(dx * dx + dy * dy));
			x += dx / length * EaseIn(q) * w * 0.8F;
			y += (dy / length * EaseIn(q) - 0.3F * q) * h * 0.8F;
			angle += (piece.seed > 0.5F ? 1.F : -1.F) * 420.F * q;
			alpha *= 1.F - q;
		}
		if (alpha <= 0.F || x + width < -20.F || x - width > w + 20.F || y + height < -20.F || y - height > h + 20.F) {
			continue;
		}
		tile(renderer, piece.clip, x, y, width, height, static_cast<double>(angle), Byte(255.F * alpha), piece.shade, Byte(flash), !piece.floor);
	}
}
void ClashArena::renderPlatform(Ui::Canvas& canvas, ska::Point<float> center, float rx, float ry, float drop, float alpha) const {
	if (drop <= 0.F || alpha <= 0.F) {
		return;
	}
	const auto h = static_cast<float>(screenSize().y);
	const auto y = center.y - (1.F - Bounce(drop)) * h * 0.4F;
	const auto spread = 1.F + 0.25F * (1.F - Clamp01(drop * 1.4F));
	Ellipse(canvas, center.x, y + ry * 0.35F, rx * 1.08F * spread, ry * 1.1F, Rgba(0, 0, 0, 90.F * alpha));
	Ellipse(canvas, center.x, y, rx * spread, ry, Rgba(40, 30, 50, 150.F * alpha));
	Ellipse(canvas, center.x, y - ry * 0.12F, rx * 0.86F * spread, ry * 0.76F, Rgba(255, 245, 220, 46.F * alpha));
	Ellipse(canvas, center.x, y - ry * 0.18F, rx * 0.6F * spread, ry * 0.5F, Rgba(255, 255, 255, 30.F * alpha));
	// The dust of its landing
	if (drop >= 1.F) {
		return;
	}
	if (drop > 0.55F) {
		const auto d = (drop - 0.55F) / 0.45F;
		for (auto i = 0; i < 24; i++) {
			const auto angle = static_cast<float>(i) * 2.F * Pi / 24.F;
			const auto radius = 1.F + d * 0.5F;
			canvas.fill(Rgba(240, 220, 190, 200.F * (1.F - d) * alpha), ska::Rectangle{ static_cast<int>(center.x + std::cos(angle) * rx * radius), static_cast<int>(center.y + std::sin(angle) * ry * radius), 6, 6 });
		}
	}
}

void ClashArena::renderFighter(Ui::Canvas& canvas, const Fighter& f, float build, float scatter, ska::Point<int> offset) const {
	const auto& turn = m_stage.turn;
	const auto screen = screenSize();
	const auto w = static_cast<float>(screen.x);
	const auto h = static_cast<float>(screen.y);
	const auto isPlayer = f.entity == turn.player;
	// Its entrance : the Pokemon of the player slides in, the opponent drops
	const auto entrance = Clamp01((build - (isPlayer ? 880.F : 960.F)) / 260.F);
	if (entrance <= 0.F) {
		return;
	}
	auto x = f.feet.x + static_cast<float>(offset.x);
	auto y = f.feet.y + static_cast<float>(offset.y);
	if (isPlayer) {
		x -= (1.F - EaseOut(entrance)) * w * 0.6F;
	} else {
		y -= (1.F - Bounce(entrance)) * h * 0.5F;
	}
	auto alpha = 255.F * Clamp01(entrance * 3.F);
	if (scatter >= 0.F) {
		alpha *= 1.F - Clamp01(scatter / 260.F);
	}
	auto flash = 0.F;
	auto column = static_cast<int>((m_time + (isPlayer ? 0U : 90U)) / 210) % 3;
	auto bob = std::sin(static_cast<float>(m_time) / 260.F + (isPlayer ? 0.F : 1.7F)) * f.scale * 1.2F;
	auto sink = 0;
	auto shade = 255.F;

	// Its skill : the wind up, the charge (a contact) or a short step (a projectile), back
	const auto t = static_cast<float>(turn.time);
	const auto other = fighter(isPlayer ? turn.opponent : turn.player);
	const auto toward = ska::Point<float>{ other.feet.x - f.feet.x, other.feet.y - f.feet.y };
	if (turn.step == Step::Acting && turn.attacker == f.entity) {
		auto move = 0.F;
		const auto reach = turn.contact ? 0.72F : 0.1F;
		if (t < 420.F) {
			move = -0.06F * EaseOut(t / 420.F);
			flash = 90.F + 70.F * std::sin(t / 35.F);
			bob = 0.F;
		} else if (t < static_cast<float>(BattleTurn::HitDelay)) {
			move = Lerp(-0.06F, reach, EaseIn((t - 420.F) / 280.F));
			column = static_cast<int>(m_time / 55) % 3;
		} else if (t < 1050.F) {
			move = reach * (1.F - EaseOut((t - static_cast<float>(BattleTurn::HitDelay)) / 350.F));
		}
		x += toward.x * move;
		y += toward.y * move;
	}
	// Hit : a white flash, it is pushed back, it blinks
	if (turn.step == Step::Acting && turn.target == f.entity && turn.hit && turn.damage > 0 && turn.hitAge < 520) {
		const auto age = static_cast<float>(turn.hitAge);
		const auto length = std::max(1.F, std::sqrt(toward.x * toward.x + toward.y * toward.y));
		const auto push = 30.F * f.scale / 3.F * (1.F - EaseOut(age / 520.F));
		x -= toward.x / length * push;
		y -= toward.y / length * push * 0.5F;
		flash = age < 100.F ? 255.F : 0.F;
		if (age < 460.F && (turn.hitAge / 60) % 2 == 1) {
			alpha *= 0.35F;
		}
	}
	// K.O. : it sinks, grey
	if (turn.knockedOut == f.entity && (turn.step == Step::KnockOut || turn.step == Step::Return)) {
		const auto since = turn.step == Step::KnockOut ? t : static_cast<float>(BattleTurn::KnockOutDuration);
		sink = static_cast<int>(std::min(72.F, since / 14.F));
		shade = 120.F;
		bob = 0.F;
		column = 0;
		if (since > 1300.F) {
			alpha *= 1.F - Clamp01((since - 1300.F) / 500.F);
		}
	}
	// In a pokeball : it turns to light and shrinks into it, it stays in it, it comes out if it breaks free ; captured : gone
	if (m_entities.hasComponent<FighterComponent>(f.entity) && m_entities.getComponent<FighterComponent>(f.entity).captured && turn.step != Step::Acting) {
		return;
	}
	auto shrink = 1.F;
	if (turn.step == Step::Acting && turn.ball && turn.target == f.entity) {
		const auto flight = static_cast<float>(BattleTurn::BallFlight);
		const auto into = static_cast<float>(BattleTurn::BallIn);
		const auto end = static_cast<float>(turn.ballEnd());
		if (t >= flight && t < into) {
			const auto p = (t - flight) / (into - flight);
			flash = 255.F;
			shrink = 1.F - EaseIn(p);
			y -= p * f.scale * 14.F;
		} else if (t >= into && (turn.caught || t < end)) {
			return;
		} else if (t >= end) {
			// Out again
			flash = 255.F * (1.F - Clamp01((t - end) / 300.F));
			shrink = EaseOutBack((t - end) / 300.F);
		}
	}
	// Its shadow
	Ellipse(canvas, x, f.feet.y + static_cast<float>(offset.y) - f.scale * 2.F, f.scale * 15.F * shrink, f.scale * 4.F * shrink, Rgba(0, 0, 0, 0.35F * alpha));
	if (shrink <= 0.01F) {
		return;
	}
	pokemon(canvas.renderer(), monster(f.entity), f.row, column, x, y + bob, f.scale * shrink, Byte(alpha), Byte(flash), sink, Byte(shade));
}

void ClashArena::renderProjectile(Ui::Canvas& canvas, ska::Point<int> offset) const {
	const auto& turn = m_stage.turn;
	if (turn.step != Step::Acting || turn.attacker == 0 || turn.contact || turn.skillSprite < 0 || turn.target == 0) {
		return;
	}
	const auto t = static_cast<float>(turn.time);
	constexpr auto From = 450.F;
	const auto to = static_cast<float>(BattleTurn::HitDelay);
	if (t < From || t >= to) {
		return;
	}
	const auto a = fighter(turn.attacker);
	const auto b = fighter(turn.target);
	const auto& texture = m_skillSprite(turn.skillSprite);
	const auto frameW = static_cast<int>(texture.getWidth()) / 2;
	const auto frameH = static_cast<int>(texture.getHeight()) / 2;
	if (frameW <= 0 || frameH <= 0) {
		return;
	}
	auto& renderer = canvas.renderer();
	// A trail of ghosts behind it
	for (auto ghost = 4; ghost >= 0; ghost--) {
		const auto p = EaseIn((t - From - static_cast<float>(ghost) * 22.F) / (to - From));
		if (p <= 0.F) {
			continue;
		}
		const auto x = Lerp(a.feet.x, b.feet.x, p) + static_cast<float>(offset.x);
		const auto y = Lerp(a.feet.y - 14.F * a.scale, b.feet.y - 14.F * b.scale, p) - std::sin(p * Pi) * 40.F + static_cast<float>(offset.y);
		const auto scale = Lerp(a.scale, b.scale, p) * 0.9F;
		const auto frame = static_cast<int>(m_time / 70) % 4;
		const auto clip = ska::Rectangle{ (frame % 2) * frameW, (frame / 2) * frameH, frameW, frameH };
		auto effect = ska::RenderEffect{};
		effect.scaleX = scale;
		effect.scaleY = scale;
		effect.anchorBottom = false;
		effect.alpha = Byte(255.F * (ghost == 0 ? 1.F : 0.5F / static_cast<float>(ghost)));
		effect.flash = Byte(ghost == 0 ? 0.F : 150.F);
		renderer.setEffect(&effect);
		renderer.render(texture, static_cast<int>(x) - frameW / 2, static_cast<int>(y) - frameH / 2, &clip, static_cast<double>(m_time % 360), nullptr);
		renderer.setEffect(nullptr);
	}
}

void ClashArena::renderHit(Ui::Canvas& canvas, ska::Point<int> offset) const {
	const auto& turn = m_stage.turn;
	if (turn.step != Step::Acting || !turn.hit || turn.target == 0 || turn.item >= 0) {
		return;
	}
	const auto screen = screenSize();
	const auto w = static_cast<float>(screen.x);
	const auto h = static_cast<float>(screen.y);
	const auto age = static_cast<float>(turn.hitAge);
	const auto target = fighter(turn.target);
	const auto cx = target.feet.x + static_cast<float>(offset.x);
	const auto cy = target.feet.y - 16.F * target.scale + static_cast<float>(offset.y);
	auto& renderer = canvas.renderer();
	if (turn.damage <= 0) {
		// A skill without damage : rings around the one it changes
		const auto who = fighter(turn.damage == 0 && turn.target != 0 ? turn.attacker : turn.target);
		for (auto ring = 0; ring < 3; ring++) {
			const auto r = (age - static_cast<float>(ring) * 120.F) * 0.25F;
			if (r <= 0.F || age > 900.F) {
				continue;
			}
			for (auto i = 0; i < 40; i++) {
				const auto angle = static_cast<float>(i) * 2.F * Pi / 40.F;
				canvas.fill(Rgba(180, 230, 255, 200.F * (1.F - age / 900.F)), ska::Rectangle{ static_cast<int>(who.feet.x + std::cos(angle) * r * who.scale / 2.F), static_cast<int>(who.feet.y - 14.F * who.scale + std::sin(angle) * r * who.scale / 4.F), 5, 5 });
			}
		}
		return;
	}
	// The sprite of a contact skill on it, the impact, the sparks, a shock ring
	if (turn.contact && turn.skillSprite >= 0 && age < 360.F) {
		const auto& texture = m_skillSprite(turn.skillSprite);
		const auto frameW = static_cast<int>(texture.getWidth()) / 2;
		const auto frameH = static_cast<int>(texture.getHeight()) / 2;
		if (frameW > 0 && frameH > 0) {
			const auto frame = std::min(3, static_cast<int>(age / 90.F));
			const auto clip = ska::Rectangle{ (frame % 2) * frameW, (frame / 2) * frameH, frameW, frameH };
			auto effect = ska::RenderEffect{};
			effect.scaleX = target.scale * 1.4F;
			effect.scaleY = target.scale * 1.4F;
			effect.anchorBottom = false;
			effect.alpha = Byte(255.F * (1.F - age / 360.F));
			renderer.setEffect(&effect);
			renderer.render(texture, static_cast<int>(cx) - frameW / 2, static_cast<int>(cy) - frameH / 2, &clip, 0., nullptr);
			renderer.setEffect(nullptr);
		}
	}
	if (age < 320.F) {
		auto effect = ska::RenderEffect{};
		const auto scale = target.scale * (0.9F + age / 260.F);
		effect.scaleX = scale;
		effect.scaleY = scale;
		effect.anchorBottom = false;
		effect.alpha = Byte(255.F * (1.F - age / 320.F));
		renderer.setEffect(&effect);
		renderer.render(m_impact, static_cast<int>(cx) - static_cast<int>(m_impact.getWidth()) / 2, static_cast<int>(cy) - static_cast<int>(m_impact.getHeight()) / 2, nullptr, static_cast<double>(turn.attacker * 47 % 360), nullptr);
		renderer.setEffect(nullptr);
	}
	if (age < 650.F) {
		for (auto i = 0; i < 22; i++) {
			const auto angle = Hash(i, static_cast<int>(turn.target), 80) * 2.F * Pi;
			const auto speed = (0.3F + Hash(i, 81) * 0.7F) * target.scale / 3.F;
			const auto x = cx + std::cos(angle) * speed * age;
			const auto y = cy + std::sin(angle) * speed * age + 0.0012F * age * age;
			const auto size = static_cast<int>(9.F * (1.F - age / 650.F)) + 1;
			canvas.fill(i % 3 == 0 ? Rgba(255, 255, 255) : Rgba(255, 210, 60), ska::Rectangle{ static_cast<int>(x), static_cast<int>(y), size, size });
		}
		const auto radius = age * 0.5F * target.scale / 3.F;
		for (auto i = 0; i < 48; i++) {
			const auto angle = static_cast<float>(i) * 2.F * Pi / 48.F;
			canvas.fill(Rgba(255, 250, 220, 220.F * (1.F - age / 650.F)), ska::Rectangle{ static_cast<int>(cx + std::cos(angle) * radius), static_cast<int>(cy + std::sin(angle) * radius * 0.6F), 4, 4 });
		}
	}
	// The damage pops, rises
	if (age < 1000.F) {
		const auto pop = 1.F + 0.9F * (1.F - EaseOut(age / 140.F));
		const auto alpha = 255.F * (1.F - Clamp01((age - 650.F) / 350.F));
		Shout(canvas, "-" + std::to_string(turn.damage), 46, Rgba(255, 235, 90), Rgba(120, 20, 0), cx + 40.F, cy - 40.F * target.scale / 3.F - age * 0.06F, pop, alpha);
	}
	// The hits in a row of the Pokemon of the player
	if (turn.combo >= 2 && turn.attacker == turn.player && age < 1100.F) {
		const auto pop = 1.F + 1.2F * (1.F - EaseOutBack(age / 220.F));
		const auto alpha = 255.F * (1.F - Clamp01((age - 850.F) / 250.F));
		Shout(canvas, std::to_string(turn.combo) + " COUPS !", 44, Rgba(255, 190, 40), Rgba(110, 10, 10), w * 0.14F, h * 0.36F, pop, alpha);
		canvas.text("COMBO", Style(20, Rgba(255, 255, 255), 2, Rgba(110, 10, 10), 1.F, alpha), static_cast<int>(w * 0.14F), static_cast<int>(h * 0.36F) + 26, Ui::Align::Center);
	}
	// The screen flashes
	canvas.fill(Rgba(255, 255, 255, 170.F * (1.F - Clamp01(age / 90.F))), ska::Rectangle{ 0, 0, screen.x, screen.y });
}

void ClashArena::renderScene(Ui::Canvas& canvas, float build, float scatter) const {
	const auto& turn = m_stage.turn;
	const auto screen = screenSize();
	const auto w = static_cast<float>(screen.x);
	const auto h = static_cast<float>(screen.y);
	const auto offset = shake();
	const auto v = view();
	// The night under it while it is built, the world again when it breaks
	const auto shown = scatter < 0.F ? 1.F : 1.F - Clamp01((scatter - 260.F) / 600.F);
	canvas.fill(Rgba(12, 8, 24, 255.F * shown), ska::Rectangle{ 0, 0, screen.x, screen.y });
	renderSky(canvas, Clamp01(build / 260.F) * shown, offset);
	// Under the ground, its rows across the whole screen (beyond the blocks : the horizon), then its tiles
	auto& renderer = canvas.renderer();
	for (const auto& row : m_rows) {
		const auto p = Clamp01((build - row.delay) / 300.F);
		const auto alpha = p * shown;
		if (alpha <= 0.F) {
			continue;
		}
		const auto far = row.forward >= static_cast<float>(FarRow) ? v.horizon : project(v, row.forward + 0.5F, 0.F, 0.F).y;
		const auto near = project(v, row.forward - 0.5F, 0.F, 0.F).y;
		tile(renderer, row.clip, w / 2.F + static_cast<float>(offset.x), (far + near) / 2.F + static_cast<float>(offset.y), w + 80.F, near - far + 1.F, 0., Byte(255.F * alpha), row.shade);
	}
	renderPieces(canvas, m_ground, build, scatter, offset, -1e9F, 1e9F);
	// The haze of the horizon
	const auto haze = Clamp01(build / 500.F) * shown;
	for (auto i = 0; i < 14; i++) {
		const auto y = static_cast<int>(v.horizon) + offset.y + i * static_cast<int>(h * 0.01F);
		canvas.fill(Rgba(255, 160, 110, 140.F * (1.F - static_cast<float>(i) / 14.F) * haze), ska::Rectangle{ 0, y, screen.x, static_cast<int>(h * 0.01F) + 1 });
	}
	const auto player = fighter(turn.player);
	const auto opponent = fighter(turn.opponent);
	const auto platformAlpha = scatter < 0.F ? 1.F : 1.F - Clamp01(scatter / 400.F);
	const auto ox = static_cast<float>(offset.x);
	const auto oy = static_cast<float>(offset.y);
	// The objects in front of the camera first (the two Pokemon are never hidden : the fight is seen), the Pokemon, then the
	// objects beside the camera
	renderPieces(canvas, m_standing, build, scatter, offset, 0.F, 1e9F);
	renderPlatform(canvas, ska::Point<float>{ opponent.feet.x + ox, opponent.feet.y + oy }, opponent.scale * 34.F, opponent.scale * 8.F, Clamp01((build - 720.F) / 140.F), platformAlpha);
	renderFighter(canvas, opponent, build, scatter, offset);
	renderPlatform(canvas, ska::Point<float>{ player.feet.x + ox, player.feet.y + oy }, player.scale * 34.F, player.scale * 8.F, Clamp01((build - 560.F) / 140.F), platformAlpha);
	renderFighter(canvas, player, build, scatter, offset);
	renderPieces(canvas, m_standing, build, scatter, offset, -1e9F, 0.F);
	renderProjectile(canvas, offset);
	renderHit(canvas, offset);
	renderItem(canvas, offset);
}

void ClashArena::renderItem(Ui::Canvas& canvas, ska::Point<int> offset) const {
	const auto& turn = m_stage.turn;
	if (turn.step != Step::Acting || turn.item < 0) {
		return;
	}
	const auto screen = screenSize();
	const auto w = static_cast<float>(screen.x);
	const auto h = static_cast<float>(screen.y);
	const auto t = static_cast<float>(turn.time);
	const auto ox = static_cast<float>(offset.x);
	const auto oy = static_cast<float>(offset.y);
	auto& renderer = canvas.renderer();
	if (!turn.ball) {
		// A healing item : green sparkles rise around the Pokemon of the player, its hit points
		if (!turn.hit || turn.healed <= 0) {
			return;
		}
		const auto age = static_cast<float>(turn.hitAge);
		const auto who = fighter(turn.attacker);
		for (auto i = 0; i < 26; i++) {
			const auto phase = std::fmod(age / 900.F + Hash(i, 90), 1.F);
			const auto x = who.feet.x + ox + (Hash(i, 91) - 0.5F) * who.scale * 34.F;
			const auto y = who.feet.y + oy - phase * who.scale * 40.F;
			const auto size = static_cast<int>(4.F + 4.F * (1.F - phase));
			canvas.fill(i % 3 == 0 ? Rgba(255, 255, 255, 230.F * (1.F - phase)) : Rgba(120, 255, 150, 230.F * (1.F - phase)), ska::Rectangle{ static_cast<int>(x), static_cast<int>(y), size, size });
		}
		const auto alpha = 255.F * (1.F - Clamp01((age - 700.F) / 300.F));
		Shout(canvas, "+" + std::to_string(turn.healed) + " PV", 44, Rgba(140, 255, 150), Rgba(0, 70, 20), who.feet.x + ox + 60.F, who.feet.y + oy - who.scale * 30.F - age * 0.05F, 1.F + 0.8F * (1.F - EaseOut(age / 140.F)), alpha);
		return;
	}
	// A pokeball : thrown from behind the camera, it swallows the opponent in its light, drops, shakes, then it clicks or
	// bursts open
	if (!m_ballLoaded) {
		m_ballLoaded = true;
		m_ballTexture = ska::Texture{ "./Sprites/Fight/pokeball.png" };
		m_burstTexture = ska::Texture{ "./Sprites/Fight/pokeball-burst.png" };
	}
	const auto target = fighter(turn.target);
	const auto frameSize = static_cast<int>(m_ballTexture.getHeight());
	if (frameSize <= 0) {
		return;
	}
	const auto scale = target.scale * 1.5F;
	const auto centre = ska::Point<float>{ target.feet.x + ox, target.feet.y + oy - target.scale * 16.F };
	const auto ground = target.feet.y + oy - static_cast<float>(frameSize) * scale * 0.5F;
	const auto flight = static_cast<float>(BattleTurn::BallFlight);
	const auto into = static_cast<float>(BattleTurn::BallIn);
	const auto drop = static_cast<float>(BattleTurn::BallDrop);
	const auto end = static_cast<float>(turn.ballEnd());
	auto x = centre.x;
	auto y = centre.y;
	auto frame = 0;
	auto angle = 0.0;
	auto shade = 255.F;
	auto flash = 0.F;
	if (t < flight) {
		// Its arc, spinning, a trail
		const auto p = t / flight;
		const auto from = ska::Point<float>{ w * 0.08F, h * 1.05F };
		x = Lerp(from.x, centre.x, p);
		y = Lerp(from.y, centre.y, p) - std::sin(p * Pi) * h * 0.35F;
		frame = static_cast<int>(t / 45.F) % 4;
		angle = static_cast<double>(t * 1.4F);
		for (auto ghost = 1; ghost <= 4; ghost++) {
			const auto q = std::max(0.F, p - 0.05F * static_cast<float>(ghost));
			canvas.fill(Rgba(255, 255, 255, 120.F / static_cast<float>(ghost)), ska::Rectangle{ static_cast<int>(Lerp(from.x, centre.x, q) - std::sin(q * Pi) * 0.F) - 4, static_cast<int>(Lerp(from.y, centre.y, q) - std::sin(q * Pi) * h * 0.35F) - 4, 8, 8 });
		}
	} else if (t < into) {
		// It opens, the light : rays from it
		const auto p = (t - flight) / (into - flight);
		for (auto i = 0; i < 12; i++) {
			const auto a = static_cast<float>(i) * 2.F * Pi / 12.F + p;
			Line(canvas, Rgba(255, 240, 200, 200.F * (1.F - p)), centre.x, centre.y, centre.x + std::cos(a) * target.scale * 40.F * p, centre.y + std::sin(a) * target.scale * 40.F * p, 3);
		}
		flash = 200.F * (1.F - p);
	} else if (t < drop) {
		// It drops on the platform, it bounces
		const auto p = (t - into) / (drop - into);
		y = Lerp(centre.y, ground, Bounce(p));
	} else if (t < end) {
		y = ground;
		const auto inShake = std::fmod(t - drop, static_cast<float>(BattleTurn::ShakeTime));
		const auto shake = static_cast<int>((t - drop) / static_cast<float>(BattleTurn::ShakeTime));
		if (shake < turn.shakes && inShake > 150.F && inShake < 550.F) {
			const auto p = (inShake - 150.F) / 400.F;
			angle = static_cast<double>(std::sin(p * 2.F * Pi) * 28.F * (1.F - p * 0.3F));
			x += std::sin(p * 2.F * Pi) * target.scale * 3.F;
		}
	} else {
		y = ground;
		const auto since = t - end;
		if (turn.caught) {
			// Click : it stays, darker, stars
			shade = 170.F;
			flash = 255.F * (1.F - Clamp01(since / 150.F));
			for (auto i = 0; i < 3; i++) {
				const auto a = -Pi / 2.F + (static_cast<float>(i) - 1.F) * 0.7F;
				const auto r = target.scale * (10.F + since * 0.05F);
				const auto alpha = 255.F * (1.F - Clamp01((since - 500.F) / 400.F));
				Shout(canvas, "*", 34, Rgba(255, 240, 120), Rgba(120, 80, 0), x + std::cos(a) * r, y + std::sin(a) * r, 1.F, alpha);
			}
			const auto pop = 1.F + 2.F * (1.F - EaseOutBack(since / 220.F));
			Shout(canvas, "GOTCHA !", 80, Rgba(255, 220, 60), Rgba(140, 20, 0), w / 2.F, h * 0.3F, pop, 255.F * (1.F - Clamp01((since - 1300.F) / 300.F)));
		} else {
			// It bursts open
			const auto p = Clamp01(since / 350.F);
			if (p < 1.F && m_burstTexture.getWidth() > 0) {
				auto effect = ska::RenderEffect{};
				effect.scaleX = scale * (0.6F + p);
				effect.scaleY = scale * (0.6F + p);
				effect.anchorBottom = false;
				effect.alpha = Byte(255.F * (1.F - p));
				renderer.setEffect(&effect);
				renderer.render(m_burstTexture, static_cast<int>(x) - static_cast<int>(m_burstTexture.getWidth()) / 2, static_cast<int>(y) - static_cast<int>(m_burstTexture.getHeight()) / 2, nullptr, 0., nullptr);
				renderer.setEffect(nullptr);
			}
			return;
		}
	}
	const auto clip = ska::Rectangle{ frame * frameSize, 0, frameSize, frameSize };
	auto effect = ska::RenderEffect{};
	effect.scaleX = scale;
	effect.scaleY = scale;
	effect.anchorBottom = false;
	effect.red = Byte(shade);
	effect.green = Byte(shade);
	effect.blue = Byte(shade);
	effect.flash = Byte(flash);
	renderer.setEffect(&effect);
	renderer.render(m_ballTexture, static_cast<int>(x) - frameSize / 2, static_cast<int>(y) - frameSize / 2, &clip, angle, nullptr);
	renderer.setEffect(nullptr);
}
void ClashArena::renderBar(Ui::Canvas& canvas, ska::EntityId entity, bool left, float shown) const {
	if (!m_entities.hasComponent<BattleComponent>(entity)) {
		return;
	}
	const auto screen = screenSize();
	const auto w = static_cast<float>(screen.x);
	const auto& battle = m_entities.getComponent<BattleComponent>(entity);
	const auto* state = m_entities.hasComponent<FighterComponent>(entity) ? &m_entities.getComponent<FighterComponent>(entity) : nullptr;
	const auto maxHp = static_cast<float>(std::max(1, battle.maxHp));
	const auto ratio = Clamp01(static_cast<float>(battle.hp) / maxHp);
	const auto trail = std::max(ratio, Clamp01((state != nullptr ? state->shownHp : static_cast<float>(battle.hp)) / maxHp));
	const auto slide = (1.F - EaseOut(shown)) * -140.F;
	const auto y = 30.F + slide;
	const auto portrait = 78.F;
	const auto x0 = left ? 24.F + portrait : w / 2.F + 62.F;
	const auto x1 = left ? w / 2.F - 62.F : w - 24.F - portrait;
	const auto barH = 26.F;
	const auto width = x1 - x0;
	// The frame, the groove
	Slanted(canvas, x0 - 5.F, y - 5.F, width + 10.F, barH + 10.F, left ? 8.F : -8.F, Rgba(10, 10, 20, 230.F));
	canvas.fill(Rgba(90, 10, 10), ska::Rectangle{ static_cast<int>(x0), static_cast<int>(y), static_cast<int>(width), static_cast<int>(barH) });
	// The part just lost (red), the hit points (yellow, orange when low) : the bars empty towards the outside
	const auto low = ratio < 0.25F;
	const auto pulse = 0.5F + 0.5F * std::sin(static_cast<float>(m_time) / 90.F);
	const auto fill = low ? Rgba(255, static_cast<int>(110.F + 60.F * pulse), 30) : Rgba(255, 214, 40);
	const auto trailW = width * trail;
	const auto keptW = width * ratio;
	if (left) {
		canvas.fill(Rgba(230, 40, 30), ska::Rectangle{ static_cast<int>(x1 - trailW), static_cast<int>(y), static_cast<int>(trailW), static_cast<int>(barH) });
		canvas.fill(fill, ska::Rectangle{ static_cast<int>(x1 - keptW), static_cast<int>(y), static_cast<int>(keptW), static_cast<int>(barH) });
		canvas.fill(Rgba(255, 255, 255, 120.F), ska::Rectangle{ static_cast<int>(x1 - keptW), static_cast<int>(y) + 3, static_cast<int>(keptW), 5 });
	} else {
		canvas.fill(Rgba(230, 40, 30), ska::Rectangle{ static_cast<int>(x0), static_cast<int>(y), static_cast<int>(trailW), static_cast<int>(barH) });
		canvas.fill(fill, ska::Rectangle{ static_cast<int>(x0), static_cast<int>(y), static_cast<int>(keptW), static_cast<int>(barH) });
		canvas.fill(Rgba(255, 255, 255, 120.F), ska::Rectangle{ static_cast<int>(x0), static_cast<int>(y) + 3, static_cast<int>(keptW), 5 });
	}
	// Its portrait (from the front), its name, its hit points
	const auto px = left ? 18.F : w - 18.F - portrait;
	const auto frameColor = left ? Rgba(60, 140, 255) : Rgba(255, 70, 60);
	canvas.fill(Rgba(10, 10, 20, 230.F), ska::Rectangle{ static_cast<int>(px) - 3, static_cast<int>(y) - 9, static_cast<int>(portrait) + 6, static_cast<int>(portrait) + 6 });
	canvas.fill(frameColor, ska::Rectangle{ static_cast<int>(px), static_cast<int>(y) - 6, static_cast<int>(portrait), static_cast<int>(portrait) });
	canvas.fill(Rgba(20, 20, 40), ska::Rectangle{ static_cast<int>(px) + 3, static_cast<int>(y) - 3, static_cast<int>(portrait) - 6, static_cast<int>(portrait) - 6 });
	pokemon(canvas.renderer(), monster(entity), left ? 7 : 6, 0, px + portrait / 2.F, y + portrait - 12.F, 1.25F, 255, 0, 0, low ? Byte(180.F + 75.F * pulse) : 255);
	const auto& name = left ? m_stage.turn.pokemon : m_stage.turn.opponentName;
	canvas.text(Upper(name), Style(22, Rgba(255, 255, 255), 2, Rgba(0, 0, 0)), static_cast<int>(left ? x0 : x1), static_cast<int>(y + barH + 8.F), left ? Ui::Align::Left : Ui::Align::Right);
	canvas.text(std::to_string(std::max(0, battle.hp)) + " / " + std::to_string(battle.maxHp), Style(16, Rgba(255, 230, 160), 2, Rgba(0, 0, 0)), static_cast<int>(left ? x1 : x0), static_cast<int>(y + barH + 10.F), left ? Ui::Align::Right : Ui::Align::Left);
}

void ClashArena::renderHud(Ui::Canvas& canvas, float shown) const {
	if (shown <= 0.F) {
		return;
	}
	const auto& turn = m_stage.turn;
	const auto screen = screenSize();
	const auto w = static_cast<float>(screen.x);
	renderBar(canvas, turn.player, true, shown);
	renderBar(canvas, turn.opponent, false, shown);
	// The timer (the time left to choose), the rounds
	const auto slide = (1.F - EaseOut(shown)) * -140.F;
	const auto box = ska::Rectangle{ static_cast<int>(w / 2.F) - 50, static_cast<int>(14.F + slide), 100, 70 };
	canvas.fill(Rgba(10, 10, 20, 235.F), box);
	canvas.fill(Rgba(255, 200, 60), ska::Rectangle{ box.x, box.y, box.w, 3 });
	canvas.fill(Rgba(255, 200, 60), ska::Rectangle{ box.x, box.y + box.h - 3, box.w, 3 });
	if (turn.step == Step::Menu) {
		const auto left = static_cast<int>(std::ceil(static_cast<float>(BattleTurn::MenuDuration - std::min(turn.time, BattleTurn::MenuDuration)) / 1000.F));
		const auto urgent = left <= 3;
		const auto beat = urgent ? 1.F + 0.15F * std::max(0.F, std::sin(static_cast<float>(m_time) / 80.F)) : 1.F;
		Shout(canvas, std::to_string(left), 50, urgent ? Rgba(255, 80, 60) : Rgba(255, 255, 255), Rgba(0, 0, 0), w / 2.F, static_cast<float>(box.y) + 34.F, beat, 255.F);
	} else {
		Shout(canvas, std::to_string(turn.round) + "/" + std::to_string(turn.rounds), 34, Rgba(255, 220, 120), Rgba(0, 0, 0), w / 2.F, static_cast<float>(box.y) + 34.F, 1.F, 255.F);
	}
	// A mark for each round : played, playing, to come
	for (auto r = 1; r <= turn.rounds; r++) {
		const auto x = static_cast<int>(w / 2.F) + (r - 1 - (turn.rounds - 1) / 2) * 22 - (turn.rounds % 2 == 0 ? 11 : 0) - 7;
		const auto y = box.y + box.h + 8;
		const auto color = r < turn.round ? Rgba(255, 200, 60) : (r == turn.round ? Rgba(255, 255, 255, 140.F + 115.F * std::sin(static_cast<float>(m_time) / 120.F)) : Rgba(70, 70, 90));
		canvas.fill(Rgba(0, 0, 0, 200.F), ska::Rectangle{ x - 2, y - 2, 18, 18 });
		canvas.fill(color, ska::Rectangle{ x, y, 14, 14 });
	}
}

void ClashArena::renderRound(Ui::Canvas& canvas) const {
	const auto& turn = m_stage.turn;
	const auto screen = screenSize();
	const auto w = static_cast<float>(screen.x);
	const auto h = static_cast<float>(screen.y);
	const auto t = static_cast<float>(turn.time);
	const auto fightAt = static_cast<float>(BattleTurn::FightAt);
	const auto out = 1.F - Clamp01((t - 1200.F) / 200.F);
	// Two bands cross the screen
	const auto band = EaseOut(t / 200.F);
	const auto y = h * 0.42F;
	Slanted(canvas, -w + band * w, y - 62.F, w, 124.F, 30.F, Rgba(0, 0, 0, 170.F * out));
	canvas.fill(Rgba(230, 40, 30, 255.F * out), ska::Rectangle{ static_cast<int>(w - band * w), static_cast<int>(y) - 66, screen.x, 4 });
	canvas.fill(Rgba(230, 40, 30, 255.F * out), ska::Rectangle{ static_cast<int>(-w + band * w), static_cast<int>(y) + 62, screen.x, 4 });
	if (t < fightAt) {
		const auto stamp = 1.F + 2.2F * (1.F - EaseOutBack((t - 80.F) / 220.F));
		const auto alpha = 255.F * Clamp01((t - 80.F) / 80.F) * (1.F - Clamp01((t - fightAt + 80.F) / 80.F));
		const auto last = turn.round == turn.rounds && turn.rounds > 1;
		Shout(canvas, last ? "ROUND FINAL" : "ROUND " + std::to_string(turn.round), 72, Rgba(255, 255, 255), Rgba(160, 20, 20), w / 2.F, y, stamp, alpha);
	} else {
		const auto since = t - fightAt;
		const auto stamp = 1.1F + 2.6F * (1.F - EaseOutBack(since / 180.F)) + 0.06F * std::sin(since / 50.F);
		Shout(canvas, "FIGHT !", 88, Rgba(255, 190, 40), Rgba(140, 20, 0), w / 2.F, y, stamp, 255.F * out);
		canvas.fill(Rgba(255, 255, 255, 200.F * (1.F - Clamp01(since / 140.F))), ska::Rectangle{ 0, 0, screen.x, screen.y });
	}
}

void ClashArena::renderMessage(Ui::Canvas& canvas, float shown) const {
	const auto& turn = m_stage.turn;
	if (turn.message.empty() || shown <= 0.F) {
		return;
	}
	const auto screen = screenSize();
	const auto w = static_cast<float>(screen.x);
	const auto h = static_cast<float>(screen.y);
	const auto in = EaseOut(static_cast<float>(turn.messageAge) / 160.F) * shown;
	const auto height = 84.F;
	const auto y = h - height * in;
	Slanted(canvas, -40.F, y, w + 80.F, height, 24.F, Rgba(10, 8, 30, 225.F));
	canvas.fill(Rgba(255, 200, 60), ska::Rectangle{ 0, static_cast<int>(y), screen.x, 3 });
	// Letter by letter (UTF-8 : not in the middle of a letter)
	auto letters = std::min(turn.message.size(), static_cast<std::size_t>(turn.messageAge / 24));
	while (letters < turn.message.size() && (static_cast<unsigned char>(turn.message[letters]) & 0xC0) == 0x80) {
		letters++;
	}
	canvas.text(turn.message.substr(0, letters), Style(28, Rgba(255, 255, 255), 2, Rgba(0, 0, 0)), 48, static_cast<int>(y) + 24, Ui::Align::Left);
}

void ClashArena::renderMenu(Ui::Canvas& canvas) const {
	const auto& turn = m_stage.turn;
	const auto screen = screenSize();
	const auto w = static_cast<float>(screen.x);
	const auto h = static_cast<float>(screen.y);
	const auto in = EaseOut(static_cast<float>(turn.time) / 200.F);
	const auto height = 168.F;
	const auto y = h - height * in;
	Slanted(canvas, -40.F, y, w + 80.F, height, 30.F, Rgba(10, 8, 30, 230.F));
	canvas.fill(Rgba(255, 200, 60), ska::Rectangle{ 0, static_cast<int>(y), screen.x, 3 });
	// The question, letter by letter
	auto letters = std::min(turn.message.size(), static_cast<std::size_t>(turn.messageAge / 24));
	while (letters < turn.message.size() && (static_cast<unsigned char>(turn.message[letters]) & 0xC0) == 0x80) {
		letters++;
	}
	canvas.text(turn.message.substr(0, letters), Style(26, Rgba(255, 255, 255), 2, Rgba(0, 0, 0)), 44, static_cast<int>(y) + 30, Ui::Align::Left);
	canvas.text(turn.bag ? "W S Entrée" : "W S Entrée  ou  1 - 4", Style(16, Rgba(190, 190, 220), 2, Rgba(0, 0, 0)), 44, static_cast<int>(y) + 72, Ui::Align::Left);

	// A box of the menu : its colour, its key, its name, its detail (the type of a skill, the amount of an item)
	const auto boxW = std::min(250.F, w * 0.2F);
	const auto boxH = 58.F;
	const auto gap = 12.F;
	const auto x0 = w - 2.F * boxW - gap - 48.F;
	const auto box = [&](std::size_t i, float x, float by, ska::Color color, const std::string& key, const std::string& name, const std::string& detail) {
		const auto selected = i == turn.selected;
		const auto pop = EaseOutBack((static_cast<float>(turn.time) - 60.F * static_cast<float>(i)) / 240.F);
		x += (1.F - pop) * 400.F;
		if (selected) {
			by -= 5.F + 2.F * std::sin(static_cast<float>(m_time) / 110.F);
		} else {
			color = Rgba(color.r * 6 / 10, color.g * 6 / 10, color.b * 6 / 10);
		}
		if (selected) {
			Slanted(canvas, x - 4.F, by - 4.F, boxW + 8.F, boxH + 8.F, 14.F, Rgba(255, 255, 255, 200.F + 55.F * std::sin(static_cast<float>(m_time) / 90.F)));
		}
		Slanted(canvas, x, by, boxW, boxH, 14.F, color);
		canvas.fill(Rgba(255, 255, 255, selected ? 90.F : 40.F), ska::Rectangle{ static_cast<int>(x) + 14, static_cast<int>(by) + 3, static_cast<int>(boxW) - 8, 6 });
		if (!key.empty()) {
			canvas.fill(Rgba(10, 10, 20, 230.F), ska::Rectangle{ static_cast<int>(x) + 14, static_cast<int>(by) + 16, 26, 26 });
			canvas.text(key, Style(18, Rgba(255, 220, 120), 0, Rgba(0, 0, 0)), static_cast<int>(x) + 27, static_cast<int>(by) + 17, Ui::Align::Center);
		}
		canvas.text(Upper(name), Style(21, Rgba(255, 255, 255), 2, Rgba(0, 0, 0)), static_cast<int>(x) + (key.empty() ? 24 : 50), static_cast<int>(by) + 8, Ui::Align::Left);
		canvas.text(Upper(detail), Style(13, Rgba(255, 255, 255, 220.F), 1, Rgba(0, 0, 0)), static_cast<int>(x + boxW) - 10, static_cast<int>(by + boxH) - 20, Ui::Align::Right);
		if (selected) {
			const auto nudge = static_cast<int>(4.F * std::sin(static_cast<float>(m_time) / 100.F));
			Shout(canvas, ">", 30, Rgba(255, 220, 60), Rgba(0, 0, 0), x - 16.F + static_cast<float>(nudge), by + boxH / 2.F, 1.F, 255.F);
		}
	};
	if (turn.bag) {
		// The items, two by two, then back
		const auto count = turn.items.size() + 1;
		const auto rows = (count + 1) / 2;
		const auto rowH = std::min(boxH + gap, (height - 26.F) / static_cast<float>(std::max<std::size_t>(rows, 2)));
		for (auto i = std::size_t{ 0 }; i < count; i++) {
			const auto x = x0 + static_cast<float>(i % 2) * (boxW + gap);
			const auto by = y + 14.F + static_cast<float>(i / 2) * rowH;
			if (i < turn.items.size()) {
				box(i, x, by, Rgba(70, 150, 110), "", turn.items[i], "x" + std::to_string(i < turn.itemCounts.size() ? turn.itemCounts[i] : 0));
			} else {
				box(i, x, by, Rgba(110, 110, 130), "", "Retour", "");
			}
		}
		return;
	}
	// The skills : two by two, the colour of their type ; the bag after them
	for (auto i = std::size_t{ 0 }; i < turn.skills.size() && i < 4; i++) {
		const auto x = x0 + static_cast<float>(i % 2) * (boxW + gap);
		const auto by = y + 20.F + static_cast<float>(i / 2) * (boxH + gap);
		const auto type = i < turn.skillTypes.size() ? turn.skillTypes[i] : std::string{};
		box(i, x, by, TypeColor(type), std::to_string(i + 1), turn.skills[i], type);
	}
	box(turn.skills.size(), x0 - boxW * 0.62F - gap - 24.F, y + 20.F + boxH + gap, Rgba(200, 120, 50), "", "Sac", "objets");
}
void ClashArena::renderKnockOut(Ui::Canvas& canvas) const {
	const auto& turn = m_stage.turn;
	const auto screen = screenSize();
	const auto w = static_cast<float>(screen.x);
	const auto h = static_cast<float>(screen.y);
	const auto t = static_cast<float>(turn.time);
	// Red and white flashes
	if (t < 480.F) {
		const auto red = (turn.time / 80) % 2 == 0;
		canvas.fill(red ? Rgba(230, 20, 20, 160.F * (1.F - t / 480.F)) : Rgba(255, 255, 255, 160.F * (1.F - t / 480.F)), ska::Rectangle{ 0, 0, screen.x, screen.y });
	}
	// Black bars
	const auto bars = static_cast<int>(EaseOut(t / 300.F) * h * 0.1F);
	canvas.fill(Rgba(0, 0, 0), ska::Rectangle{ 0, 0, screen.x, bars });
	canvas.fill(Rgba(0, 0, 0), ska::Rectangle{ 0, screen.y - bars, screen.x, bars });
	const auto stamp = 1.4F + 3.F * (1.F - EaseOut(t / 160.F));
	const auto alpha = 255.F * (1.F - Clamp01((t - 1600.F) / 400.F));
	Shout(canvas, "K.O.", 110, Rgba(235, 30, 25), Rgba(255, 220, 60), w / 2.F, h * 0.4F, stamp, alpha);
	const auto won = turn.knockedOut != turn.player;
	const auto line = Clamp01((t - 500.F) / 250.F);
	canvas.text(won ? Upper(turn.pokemon) + " GAGNE !" : Upper(turn.pokemon) + " EST K.O. !", Style(32, Rgba(255, 255, 255), 3, Rgba(0, 0, 0), 1.F, 255.F * line * alpha / 255.F), static_cast<int>(w / 2.F), static_cast<int>(h * 0.4F) + 84, Ui::Align::Center);
}
