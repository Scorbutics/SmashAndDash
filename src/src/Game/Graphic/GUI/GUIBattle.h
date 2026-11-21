#pragma once
#include <array>
#include <map>
#include <string>
#include "Core/Draw/DrawableFixedPriority.h"
#include "Core/ECS/ECSDefines.h"
#include "Core/Point.h"
#include "Graphic/Texture.h"
#include "Graphic/Shader.h"
#include "../../Gameplay/Fight/BattleStage.h"

class BattleSystem;
class FighterComponent;

namespace ska {
	class CameraSystem;
	class EntityManager;
	class DrawableContainer;
	class Renderer;
}

// Interface of a battle, as a fighting game : the bars of the two fighters at the top of the screen (the player on the
// left, the wild Pokemon on the right), the skills of the player at the bottom, the big announces ("COMBAT !", "K.O. !").
// In the map : the arena (an overlay : the player can leave it to flee), a ring under each fighter (blue : the player,
// red : the opponent), an arrow over the Pokemon of the player, the damage and the impacts of the hits.
class GUIBattle {
public:
	GUIBattle(ska::EntityManager& em, BattleStage& stage, const BattleSystem& battle, ska::CameraSystem& camera);
	GUIBattle(const GUIBattle&) = delete;
	GUIBattle& operator=(const GUIBattle&) = delete;
	~GUIBattle();

	void graphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables);

	void renderGround(ska::Renderer& renderer) const;
	void renderOverlay(ska::Renderer& renderer) const;

private:
	// A drawing at a fixed priority, done by the interface
	class Layer : public ska::DrawableFixedPriority {
	public:
		Layer(const GUIBattle& gui, bool ground) : m_gui(gui), m_ground(ground) {}
		void render(ska::Renderer& renderer) const override {
			if (m_ground) {
				m_gui.renderGround(renderer);
			} else {
				m_gui.renderOverlay(renderer);
			}
		}
	private:
		const GUIBattle& m_gui;
		bool m_ground;
	};

	struct Sprite {
		ska::Point<int> position;
		int width = 0;
		int height = 0;
	};

	bool visible(ska::EntityId fighter) const;
	// The drawn sprite of a fighter on the screen
	Sprite sprite(ska::EntityId fighter) const;
	ska::Point<int> toScreen(ska::Point<float> world) const;

	void renderArena(ska::Renderer& renderer) const;
	// The panel of a fighter : on the left for the side of the player, on the right for the other one, "slot" : its rank
	// on its side (the panels are stacked)
	void renderPanel(ska::Renderer& renderer, ska::EntityId entity, bool left, int slot) const;
	const std::string& name(ska::EntityId entity) const;
	// The sprite of a skill (Sprites/Fight/<id>.png : 2 x 2 frames), loaded at its first use
	const ska::Texture& skillSprite(int id) const;
	// The state of a fighter, nullptr when it does not fight (anymore)
	const FighterComponent* fighter(ska::EntityId entity) const;
	void renderSkills(ska::Renderer& renderer) const;
	void renderBanner(ska::Renderer& renderer) const;
	void renderMarks(ska::Renderer& renderer) const;
	void updateScreenShader(ska::Renderer& renderer) const;
	void text(ska::Renderer& renderer, const std::string& value, unsigned int size, ska::Color color, int x, int y, int align, float scale = 1.F, unsigned char alpha = 255) const;

	ska::EntityManager& m_entities;
	BattleStage& m_stage;
	const BattleSystem& m_battle;
	ska::CameraSystem& m_camera;
	Layer m_groundLayer;
	Layer m_overlayLayer;
	// The names of the fighters (loaded at their first drawing)
	mutable std::map<ska::EntityId, std::string> m_names;
	ska::Texture m_ringPlayer;
	ska::Texture m_ringOpponent;
	ska::Texture m_arrow;
	ska::Texture m_impact;
	// The sprites of the contact skills (Sprites/Fight/<id>.png : 2 x 2 frames), loaded when used
	mutable std::map<int, ska::Texture> m_skillSprites;
	// Post-processing of the battle (Shaders/battle) : the colors split on the hits, gray and vignette on a K.O.
	mutable ska::Shader m_screenShader;
	mutable ska::Renderer* m_renderer = nullptr;
	unsigned int m_time = 0;
};
