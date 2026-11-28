#pragma once
#include <array>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <vector>
#include "Canvas.h"
#include "Core/ECS/ECSDefines.h"
#include "Core/Point.h"
#include "Core/Rectangle.h"
#include "Graphic/Texture.h"
#include "../../Gameplay/Fight/BattleStage.h"

class WorldState;

namespace ska {
	class CameraSystem;
	class EntityManager;
	class Tileset;
	class Renderer;
}

// The drawing of a clash (BattleStage::turn, ClashSystem), a cross-over of the first games and the fighting games, over
// the screen (not zoomed with the world) :
//   Impact   the flash, the focus lines, the bolts and the sparks between the two Pokemon, "CLASH !" stamped
//   Zoom     (the camera dives : BattleCameraSystem) black bars, the vignette, a heartbeat
//   Shatter  the tiles of the map around them break away, from the middle outwards, to the night behind
//   Versus   the screen "VS" : the two sides slide in, the two Pokemon big, their names, "VS" lands
//   Rebuild  a battleground of those tiles flies in : the sky, the scenery (the trees, the houses of the map behind the
//            opponent), the ground in perspective (seen from behind the Pokemon of the player), the two platforms land
//   Round    the Pokemon of the player from behind, the opponent in front, the bars of the fighting games, "ROUND 1",
//            "FIGHT !"
//   Menu     the skills as in the first games, a timer as in the fighting games
//   Acting   the message, the wind up, the charge or the projectile, the hit (flash, shake, sparks, the damage)
//   KnockOut "K.O." : the Pokemon sinks
//   Return   the battleground breaks, the map is there again
// The tiles are those of the map (the tileset, the three layers), the Pokemon their character sheets. The battleground is
// in perspective as the racing games of the arcades : the flat tiles on the ground, the tiles that stand (walls, roofs,
// trees : their depth, Tile properties) upright on the block of their object, scaled by their distance, sorted with the
// two Pokemon ; the camera drifts sideways (the near things slide faster).
class ClashArena {
public:
	ClashArena(ska::EntityManager& em, const BattleStage& stage, ska::CameraSystem& camera, WorldState& worldState,
		std::function<const ska::Texture&(int)> skillSprite, const ska::Texture& impact);
	ClashArena(const ClashArena&) = delete;
	ClashArena& operator=(const ClashArena&) = delete;

	void update(unsigned int ellapsedTime) { m_time += ellapsedTime; }
	void render(ska::Renderer& renderer) const;

private:
	// A block of the map around the clash : its tiles (ground, middle, top : their clips in the tileset of its chunk) and
	// their depth (Tile properties : -1 flat, n standing on an object n blocks lower)
	struct Cell {
		int x = 0;
		int y = 0;
		// Its tileset (m_sheets)
		int sheet = 0;
		std::array<std::optional<ska::Rectangle>, 3> clips;
		std::array<int, 3> bases{ -1, -1, -1 };
	};
	// A tile of the battleground : where it is (blocks in front of the Pokemon of the player, to its right, how high),
	// flat on the ground or standing, its clip, its tint, when it comes
	struct Piece {
		ska::Rectangle clip{};
		int sheet = 0;
		float forward = 0.F;
		float lateral = 0.F;
		float up = 0.F;
		bool floor = false;
		unsigned char shade = 255;
		float delay = 0.F;
		float seed = 0.F;
	};
	struct Fighter {
		ska::Point<float> feet;
		float scale = 1.F;
		int row = 0;
		float forward = 0.F;
		ska::EntityId entity = 0;
	};
	// The perspective of the battleground for this frame : the horizon, the focal (pixels a block at one block), where
	// the camera is to the right, where the opponent stands
	struct View {
		float width = 0.F;
		float horizon = 0.F;
		float focal = 1.F;
		float lateral = 0.F;
		float opponentForward = 0.F;
		float opponentLateral = 0.F;
	};

	// The tiles around the clash, once a turn
	void capture() const;
	const Cell* cell(int x, int y) const;
	const Cell* wrapped(int x, int y) const;
	View view() const;
	// A point of the battleground on the screen ("size" : the pixels of a block there)
	ska::Point<float> project(const View& v, float forward, float lateral, float up, float* size = nullptr) const;
	// The battleground (its pieces) for this screen
	void layout(int width, int height) const;
	// The tiles that stand without the ground drawn in them (the bottom of a trunk, of a wall : the ground and the shadow
	// around them, as their neighbour shows it) : a copy of the tileset, those pixels transparent
	void cutGround(const std::map<std::array<int, 3>, std::vector<ska::Rectangle>>& grounds) const;
	void cutGround(int sheet, const std::map<std::array<int, 3>, std::vector<ska::Rectangle>>& grounds) const;

	ska::Point<int> screenSize() const;
	ska::Point<float> toScreen(ska::Point<float> world) const;
	ska::Point<int> shake() const;

	void renderImpact(Ui::Canvas& canvas) const;
	void renderShatter(Ui::Canvas& canvas) const;
	void renderVersus(Ui::Canvas& canvas) const;
	// The battleground : "build" ms since it is rebuilt, "scatter" ms since it breaks (< 0 : not yet)
	void renderScene(Ui::Canvas& canvas, float build, float scatter) const;
	void renderSky(Ui::Canvas& canvas, float alpha, ska::Point<int> offset) const;
	// The pieces from "from" (included) to "to" blocks in front
	void renderPieces(Ui::Canvas& canvas, const std::vector<Piece>& pieces, float build, float scatter, ska::Point<int> offset, float from, float to) const;
	void renderPlatform(Ui::Canvas& canvas, ska::Point<float> center, float rx, float ry, float drop, float alpha) const;
	void renderFighter(Ui::Canvas& canvas, const Fighter& fighter, float build, float scatter, ska::Point<int> offset) const;
	void renderProjectile(Ui::Canvas& canvas, ska::Point<int> offset) const;
	void renderHit(Ui::Canvas& canvas, ska::Point<int> offset) const;
	// An item of the bag in a round : the healing, the pokeball (its course, its shakes, caught or open)
	void renderItem(Ui::Canvas& canvas, ska::Point<int> offset) const;
	void renderHud(Ui::Canvas& canvas, float shown) const;
	void renderBar(Ui::Canvas& canvas, ska::EntityId entity, bool left, float shown) const;
	void renderRound(Ui::Canvas& canvas) const;
	void renderMenu(Ui::Canvas& canvas) const;
	void renderMessage(Ui::Canvas& canvas, float shown) const;
	void renderKnockOut(Ui::Canvas& canvas) const;

	// A tile of the tileset, its centre at x y, its size, turned, faded, tinted
	void tile(ska::Renderer& renderer, int sheet, const ska::Rectangle& clip, float x, float y, float width, float height, double angle, unsigned char alpha, unsigned char shade = 255, unsigned char flash = 0, bool standing = false) const;
	// A frame of the character sheet of a Pokemon (Sprites/Charsets), its feet at x y
	void pokemon(ska::Renderer& renderer, int monster, int row, int column, float x, float y, float scale, unsigned char alpha, unsigned char flash = 0, int sink = 0, unsigned char shade = 255) const;
	const ska::Texture& charset(int monster) const;
	int monster(ska::EntityId entity) const;
	// Where a Pokemon stands on the battleground
	Fighter fighter(ska::EntityId entity) const;

	ska::EntityManager& m_entities;
	const BattleStage& m_stage;
	ska::CameraSystem& m_camera;
	WorldState& m_worldState;
	std::function<const ska::Texture&(int)> m_skillSprite;
	const ska::Texture& m_impact;
	unsigned int m_time = 0;

	mutable std::map<int, ska::Texture> m_charsets;
	// The turn of the tiles kept, the block in the middle, the blocks around
	mutable unsigned int m_captured = 0;
	mutable int m_centerX = 0;
	mutable int m_centerY = 0;
	mutable int m_clashX = 0;
	mutable int m_clashY = 0;
	mutable ska::Point<int> m_forward{ 0, -1 };
	mutable std::vector<Cell> m_cells;
	// The battleground laid out for this screen size
	mutable ska::Point<int> m_laidOut{};
	mutable std::vector<Piece> m_rows;
	mutable std::vector<Piece> m_ground;
	mutable std::vector<Piece> m_standing;
	// The tilesets of the chunks of the blocks kept
	mutable std::vector<const ska::Tileset*> m_sheets;
	// The tilesets without the ground in the tiles that stand (cutGround), when they could be made
	mutable std::map<int, ska::Texture> m_standingTiles;
	mutable ska::Texture m_ballTexture;
	mutable ska::Texture m_burstTexture;
	mutable bool m_ballLoaded = false;
};
