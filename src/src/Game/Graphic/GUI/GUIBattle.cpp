#include "Game/__internalConfig/LoggerConfig.h"
#include "GUIBattle.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <SDL3/SDL.h>
#include "Base/IO/Readers/IniReader.h"
#include "Core/Draw/DrawableContainer.h"
#include "Core/Draw/Renderer.h"
#include "Core/ECS/EntityManager.h"
#include "Core/ECS/Basics/Physic/PositionComponent.h"
#include "Core/ECS/Basics/Physic/HitboxComponent.h"
#include "Graphic/GraphicComponent.h"
#include "Graphic/System/CameraSystem.h"
#include "../../Gameplay/Fight/BattleComponent.h"
#include "../../Gameplay/Fight/FighterComponent.h"
#include "../../Gameplay/Fight/BattleEffects.h"
#include "../../Gameplay/Fight/System/FighterSystem.h"
#include "../../Gameplay/Fight/SkillsHolderComponent.h"
#include "../../Gameplay/Fight/System/BattleSystem.h"
#include "../../Utils/IDs.h"
#include "Theme.h"

namespace {
	constexpr int GroundPriority = 9;

	std::string Upper(std::string text) {
		std::transform(text.begin(), text.end(), text.begin(), [](char c) {
			const auto u = static_cast<unsigned char>(c);
			// Latin-1 lower case letters with accents are 0xE0 - 0xFE (except 0xF7)
			if (u >= 0xE0 && u != 0xF7 && u != 0xFF) {
				return static_cast<char>(u - 0x20);
			}
			return static_cast<char>(std::toupper(u));
		});
		return text;
	}

	void FillRect(ska::Renderer& renderer, ska::Color color, int x, int y, int w, int h) {
		if (w > 0 && h > 0) {
			renderer.drawColorRect(color, ska::Rectangle{ x, y, w, h });
		}
	}

	const ska::Color PlayerColor{ 60, 150, 255, 255 };
	const ska::Color OpponentColor{ 255, 70, 60, 255 };
	// The text on the panels of the theme (Theme) : the ink of the tiles, white on the colored pills
	const ska::Color Ink{ 34, 40, 54, 255 };
	const ska::Color White{ 255, 255, 255, 255 };
}

GUIBattle::GUIBattle(ska::EntityManager& em, BattleStage& stage, const BattleSystem& battle, ska::CameraSystem& camera) :
	m_entities(em),
	m_stage(stage),
	m_battle(battle),
	m_camera(camera),
	m_groundLayer(*this, Layer::Kind::Ground),
	m_worldOverlayLayer(*this, Layer::Kind::WorldOverlay),
	m_overlayLayer(*this, Layer::Kind::Interface),
	m_ringPlayer("./Sprites/Fight/ring-player.png"),
	m_ringOpponent("./Sprites/Fight/ring-opponent.png"),
	m_arrow("./Sprites/Fight/arrow-player.png"),
	m_impact("./Sprites/Fight/impact.png"),
	m_screenShader("./Shaders/battle", true) {
	m_groundLayer.setPriority(GroundPriority);
	m_worldOverlayLayer.setPriority(WorldOverlayPriority);
	// Under the dialogs (GUI)
	m_overlayLayer.setPriority(GUI_DEFAULT_DISPLAY_PRIORITY - 1);
}

GUIBattle::~GUIBattle() {
	// The screen is not post-processed anymore (the shader is destroyed with the interface)
	if (m_renderer != nullptr) {
		m_renderer->setScreenShader(nullptr);
	}
}

void GUIBattle::updateScreenShader(ska::Renderer& renderer) const {
	m_renderer = &renderer;
	// The strongest recent impact splits the colors, a K.O. turns the screen gray
	auto split = 0.F;
	for (const auto& mark : m_stage.marks()) {
		if (mark.kind == BattleMark::Kind::Impact && mark.value > 0) {
			const auto fading = std::max(0.F, 1.F - static_cast<float>(mark.age) / 220.F);
			split = std::max(split, fading * (3.F + static_cast<float>(mark.value) * 1.2F));
		}
	}
	auto gray = 0.F;
	auto vignette = 0.F;
	if (m_stage.phase() == BattlePhase::KnockOut) {
		const auto progress = std::min(1.F, static_cast<float>(m_stage.phaseTime()) / 500.F);
		gray = 0.85F * progress;
		vignette = 0.8F * progress;
	}
	if (split <= 0.F && gray <= 0.F) {
		renderer.setScreenShader(nullptr);
		return;
	}
	const auto screen = m_camera.getScreenSize();
	struct {
		float effects[4];
		float screen[4];
	} uniforms{ { gray, vignette, split, 0.F }, { static_cast<float>(screen.x), static_cast<float>(screen.y), 0.F, 0.F } };
	m_screenShader.setUniforms(uniforms);
	renderer.setScreenShader(&m_screenShader);
}

const std::string& GUIBattle::name(ska::EntityId entity) const {
	auto found = m_names.find(entity);
	if (found == m_names.end()) {
		const auto& bc = m_entities.getComponent<BattleComponent>(entity);
		const auto data = ska::IniReader{ "./Data/Monsters/" + std::to_string(bc.monster) + ".ini" };
		found = m_names.emplace(entity, Upper(data.exists("Description name") ? data.get<std::string>("Description name") : "?")).first;
	}
	return found->second;
}

void GUIBattle::graphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) {
	m_time += ellapsedTime;
	// Before the fight and after its end (back to the map) : nothing
	if (m_stage.phase() == BattlePhase::Intro || m_stage.phase() == BattlePhase::Over) {
		return;
	}
	drawables.add(m_groundLayer);
	drawables.add(m_worldOverlayLayer);
	drawables.add(m_overlayLayer);
}

const FighterComponent* GUIBattle::fighter(ska::EntityId entity) const {
	return m_entities.hasComponent<FighterComponent>(entity) ? &m_entities.getComponent<FighterComponent>(entity) : nullptr;
}

bool GUIBattle::visible(ska::EntityId fighter) const {
	return m_entities.hasComponent<ska::PositionComponent>(fighter) && m_entities.hasComponent<ska::HitboxComponent>(fighter) && m_entities.hasComponent<ska::GraphicComponent>(fighter);
}

GUIBattle::Sprite GUIBattle::sprite(ska::EntityId fighter) const {
	// As the GraphicSystem draws it
	auto& graphic = m_entities.getComponent<ska::GraphicComponent>(fighter);
	const auto& hc = m_entities.getComponent<ska::HitboxComponent>(fighter);
	const auto drawn = graphic.drawnPosition(m_entities.getComponent<ska::PositionComponent>(fighter));
	const auto& camera = m_camera.getDisplay();
	auto result = Sprite{};
	result.position.x = static_cast<int>(std::lround(drawn.x)) - static_cast<int>(hc.width / 2) + hc.xOffset / 2 - camera.x;
	result.position.y = static_cast<int>(std::lround(drawn.y)) - static_cast<int>(hc.height / 2) - 5 - camera.y - static_cast<int>(std::lround(drawn.z));
	if (!graphic.animatedSprites.empty()) {
		result.width = static_cast<int>(graphic.animatedSprites[0].getWidth());
		result.height = static_cast<int>(graphic.animatedSprites[0].getHeight());
	}
	return result;
}

ska::Point<int> GUIBattle::toScreen(ska::Point<float> world) const {
	const auto& camera = m_camera.getDisplay();
	return ska::Point<int>{ static_cast<int>(world.x) - camera.x, static_cast<int>(world.y) - camera.y };
}

void GUIBattle::text(ska::Renderer& renderer, const std::string& value, unsigned int size, ska::Color color, int x, int y, int align, float scale, unsigned char alpha, bool outlined) const {
	if (value.empty()) {
		return;
	}
	auto texture = ska::Texture{};
	texture.loadFromText(size, value, color);
	auto outline = ska::Texture{};
	outline.loadFromText(size, value, ska::Color{ 0, 0, 0, 255 });
	const auto width = static_cast<int>(texture.getWidth());
	const auto left = align < 0 ? x : (align == 0 ? x - width / 2 : x - width);

	auto effect = ska::RenderEffect{};
	effect.scaleX = scale;
	effect.scaleY = scale;
	effect.anchorBottom = false;
	effect.alpha = alpha;
	renderer.setEffect(&effect);
	// An outline : readable over the map
	const int offsets[][2] = { { -2, 0 }, { 2, 0 }, { 0, -2 }, { 0, 2 }, { 2, 2 } };
	for (const auto& offset : offsets) {
		if (outlined) {
			renderer.render(outline, left + offset[0], y + offset[1], nullptr);
		}
	}
	renderer.render(texture, left, y, nullptr);
	renderer.setEffect(nullptr);
}

void GUIBattle::renderArena(ska::Renderer& renderer) const {
	const auto& arena = m_stage.arena;
	const auto topLeft = toScreen(ska::Point<float>{ static_cast<float>(arena.x), static_cast<float>(arena.y) });
	const auto x = topLeft.x;
	const auto y = topLeft.y;
	const auto w = arena.w;
	const auto h = arena.h;

	// Leaving it (fleeing) : the edges turn red
	const auto* player = fighter(m_stage.player);
	const auto leaving = player != nullptr && player->outside > 0;
	const auto pulse = static_cast<int>(60.F + 60.F * std::sin(static_cast<float>(m_time) / 120.F));
	const auto edge = leaving ? ska::Color{ 255, 80, 60, static_cast<uint8_t>(140 + pulse) } : ska::Color{ 255, 255, 255, 150 };
	const auto floor = leaving ? ska::Color{ 255, 60, 40, 26 } : ska::Color{ 255, 255, 255, 20 };

	FillRect(renderer, floor, x, y, w, h);
	// Dashed edges : an overlay, not a wall
	constexpr auto dash = 18;
	constexpr auto gap = 12;
	for (auto i = 0; i < w; i += dash + gap) {
		FillRect(renderer, edge, x + i, y, std::min(dash, w - i), 2);
		FillRect(renderer, edge, x + i, y + h - 2, std::min(dash, w - i), 2);
	}
	for (auto i = 0; i < h; i += dash + gap) {
		FillRect(renderer, edge, x, y + i, 2, std::min(dash, h - i));
		FillRect(renderer, edge, x + w - 2, y + i, 2, std::min(dash, h - i));
	}
	// Corners
	constexpr auto corner = 30;
	constexpr auto thick = 4;
	FillRect(renderer, edge, x - 1, y - 1, corner, thick);
	FillRect(renderer, edge, x - 1, y - 1, thick, corner);
	FillRect(renderer, edge, x + w - corner + 1, y - 1, corner, thick);
	FillRect(renderer, edge, x + w - thick + 1, y - 1, thick, corner);
	FillRect(renderer, edge, x - 1, y + h - thick + 1, corner, thick);
	FillRect(renderer, edge, x - 1, y + h - corner + 1, thick, corner);
	FillRect(renderer, edge, x + w - corner + 1, y + h - thick + 1, corner, thick);
	FillRect(renderer, edge, x + w - thick + 1, y + h - corner + 1, thick, corner);
}

void GUIBattle::renderGround(ska::Renderer& renderer) const {
	renderer.setRenderDrawBlendMode(SDL_BLENDMODE_BLEND);
	renderArena(renderer);
	renderer.setRenderDrawBlendMode(SDL_BLENDMODE_NONE);

	// A ring under each fighter : blue for the player, red for the opponent
	for (const auto entity : m_stage.fighters) {
		const auto* state = fighter(entity);
		if (state == nullptr || !visible(entity) || state->knockedOut) {
			continue;
		}
		const auto drawn = sprite(entity);
		const auto& ring = state->player ? m_ringPlayer : m_ringOpponent;
		const auto ringX = drawn.position.x + drawn.width / 2 - static_cast<int>(ring.getWidth()) / 2;
		const auto ringY = drawn.position.y + drawn.height - static_cast<int>(ring.getHeight()) / 2 - 6;
		renderer.render(ring, ringX, ringY, nullptr);
	}
}

void GUIBattle::renderPanel(ska::Renderer& renderer, ska::EntityId entity, bool left, int slot) const {
	const auto* state = fighter(entity);
	if (state == nullptr || !m_entities.hasComponent<BattleComponent>(entity)) {
		return;
	}
	const auto& fighter = *state;
	const auto& bc = m_entities.getComponent<BattleComponent>(entity);
	const auto screen = m_camera.getScreenSize();
	const auto margin = 14;
	const auto width = std::min(440, screen.x / 2 - 70);
	const auto height = 84;
	// The bar shakes after a hit
	const auto shake = fighter.barShake > 0 ? static_cast<int>((fighter.barShake / 25) % 2 == 0 ? 1 : -1) * static_cast<int>(1 + fighter.barShake / 80) : 0;
	const auto x = (left ? margin : screen.x - margin - width) + shake;
	const auto y = margin + slot * (height + 8);

	// A panel of the theme
	Theme::panel(renderer, "panel_small.png", ska::Rectangle{ x, y, width, height }, 16, 240);

	// Who it is : the tag (a pill of its side) and the name
	const auto tag = fighter.player ? std::string{ "VOUS" } : std::string{ "SAUVAGE" };
	const auto label = name(entity) + "  Nv." + std::to_string(bc.level) + (fighter.knockedOut ? "  K.O." : "");
	const auto tagWidth = fighter.player ? 62 : 92;
	const auto tagX = left ? x + 12 : x + width - 12 - tagWidth;
	Theme::panel(renderer, fighter.player ? "panel_small_teal.png" : "panel_small_red.png", ska::Rectangle{ tagX, y + 9, tagWidth, 24 }, 8);
	text(renderer, tag, 14, White, tagX + tagWidth / 2, y + 11, 0, 1.F, 255, false);
	text(renderer, label, 18, Ink, left ? tagX + tagWidth + 10 : tagX - 10, y + 9, left ? -1 : 1, 1.F, 255, false);

	// The hit points : a bar in the dark groove of the theme ; it empties towards the middle of the screen, the lost part
	// stays a moment
	const auto barX = x + 14;
	const auto barY = y + 40;
	const auto barWidth = width - 28;
	const auto barHeight = 18;
	Theme::panel(renderer, "panel_small_dark.png", ska::Rectangle{ barX - 4, barY - 4, barWidth + 8, barHeight + 8 }, 8);
	const auto maxHp = static_cast<float>(std::max(1, bc.maxHp));
	const auto hp = std::max(0.F, static_cast<float>(bc.hp));
	const auto shown = std::max(hp, fighter.shownHp);
	const auto hpWidth = static_cast<int>(static_cast<float>(barWidth) * hp / maxHp);
	const auto shownWidth = static_cast<int>(static_cast<float>(barWidth) * shown / maxHp);
	const auto ratio = hp / maxHp;
	const auto fill = ratio > 0.5F ? ska::Color{ 76, 200, 96, 255 } : (ratio > 0.25F ? ska::Color{ 240, 196, 52, 255 } : ska::Color{ 226, 64, 56, 255 });
	const auto lostX = left ? barX : barX + barWidth - shownWidth;
	const auto hpX = left ? barX : barX + barWidth - hpWidth;
	FillRect(renderer, ska::Color{ 255, 236, 200, 255 }, lostX, barY, shownWidth, barHeight);
	FillRect(renderer, fill, hpX, barY, hpWidth, barHeight);
	FillRect(renderer, ska::Color{ 255, 255, 255, 110 }, hpX, barY, hpWidth, 5);
	FillRect(renderer, ska::Color{ 0, 0, 0, 50 }, hpX, barY + barHeight - 4, hpWidth, 4);
	if (fighter.player) {
		text(renderer, std::to_string(std::max(0, bc.hp)) + " / " + std::to_string(bc.maxHp), 14, White, barX + barWidth - 6, barY + 1, 1);
	}

	// Its status and the changes of its statistics
	auto tags = StatusShortName(fighter.status);
	const char* stats[] = { "ATQ", "DEF", "VIT" };
	for (std::size_t stat = 0; stat < fighter.stages.size(); stat++) {
		const auto stage = fighter.stages[stat];
		if (stage != 0) {
			tags += (tags.empty() ? "" : "  ") + std::string{ stats[stat] } + (stage > 0 ? std::string(static_cast<std::size_t>(stage), '+') : std::string(static_cast<std::size_t>(-stage), '-'));
		}
	}
	if (fighter.guard > 0) {
		tags += (tags.empty() ? "" : "  ") + std::string{ "ABRI" };
	}
	if (!tags.empty()) {
		text(renderer, tags, 13, ska::Color{ 168, 74, 24, 255 }, left ? barX : barX + barWidth, barY + barHeight + 6, left ? -1 : 1, 1.F, 255, false);
	}

	// Fleeing : the time left outside of the arena
	if (fighter.player && fighter.outside > 0 && m_stage.phase() == BattlePhase::Fight) {
		const auto progress = std::min(1.F, static_cast<float>(fighter.outside) / static_cast<float>(BattleStage::FleeDelay));
		Theme::panel(renderer, "panel_small_dark.png", ska::Rectangle{ x, y + height + 4, width / 2, 30 }, 8);
		FillRect(renderer, ska::Color{ 255, 90, 60, 255 }, x + 6, y + height + 26, static_cast<int>(static_cast<float>(width / 2 - 12) * progress), 4);
		text(renderer, "FUITE...", 16, White, x + 12, y + height + 7, -1, 1.F, 255, false);
	}
}

void GUIBattle::renderSkills(ska::Renderer& renderer) const {
	const auto* player = fighter(m_stage.player);
	if (player == nullptr || !m_entities.hasComponent<SkillsHolderComponent>(m_stage.player) || player->controlledByAI) {
		return;
	}
	const auto& skills = m_entities.getComponent<SkillsHolderComponent>(m_stage.player).skills;
	const auto screen = m_camera.getScreenSize();
	const auto slotWidth = 176;
	const auto slotHeight = 44;
	const auto gap = 10;
	auto count = 0;
	for (const auto& skill : skills) {
		count += skill.particleNumber > 0 ? 1 : 0;
	}
	// Centred in the room left by the action buttons of the map (bottom right)
	auto x = (screen.x - 300) / 2 - (count * slotWidth + count * gap + 132) / 2;
	x = std::max(14, x);
	const auto y = screen.y - slotHeight - 14;
	// A slot of the theme : a panel, its key in a pill (teal : ready), the cooldown a shade shrinking over it
	auto slotPanel = [&](int sx, int width, int keyWidth, const std::string& key, const std::string& label, float cooldown) {
		const auto ready = cooldown <= 0.F;
		Theme::panel(renderer, "panel_small.png", ska::Rectangle{ sx, y, width, slotHeight }, 16, 240);
		Theme::panel(renderer, ready ? "panel_small_teal.png" : "panel_small_dark.png", ska::Rectangle{ sx + 5, y + 5, keyWidth, slotHeight - 10 }, 8);
		text(renderer, key, key.size() > 1 ? 15 : 20, White, sx + 5 + keyWidth / 2, y + (key.size() > 1 ? 13 : 10), 0, 1.F, 255, false);
		text(renderer, label, 15, ready ? Ink : ska::Color{ 130, 134, 144, 255 }, sx + keyWidth + 14, y + 13, -1, 1.F, 255, false);
		FillRect(renderer, ska::Color{ 34, 40, 54, 90 }, sx + keyWidth + 8, y + 5, static_cast<int>(static_cast<float>(width - keyWidth - 14) * cooldown), slotHeight - 10);
	};
	for (auto index = 0u; index < skills.size(); index++) {
		const auto& skill = skills[index];
		if (skill.particleNumber <= 0) {
			continue;
		}
		slotPanel(x, slotWidth, 30, std::to_string(index + 1), Upper(skill.name), m_battle.cooldown(index, m_stage.player));
		x += slotWidth + gap;
	}
	// The dodge (space key)
	slotPanel(x, 132, 46, "ESP", "ESQUIVE", static_cast<float>(player->dashCooldown) / static_cast<float>(FighterSystem::DashCooldown));
}

void GUIBattle::renderBanner(ska::Renderer& renderer) const {
	const auto& banner = m_stage.banner();
	if (banner.empty()) {
		return;
	}
	const auto age = m_stage.bannerAge();
	const auto lasting = m_stage.phase() == BattlePhase::KnockOut || m_stage.phase() == BattlePhase::Over;
	if (!lasting && age > 1200) {
		return;
	}
	// It hits the screen (big, then normal), then fades
	const auto scale = age < 160 ? 2.2F - 1.2F * static_cast<float>(age) / 160.F : 1.F;
	const auto alpha = !lasting && age > 900 ? static_cast<unsigned char>(255 * (1200 - age) / 300) : static_cast<unsigned char>(255);
	const auto color = banner == "K.O. !" ? ska::Color{ 255, 70, 50, 255 } : (banner == "FUITE !" ? ska::Color{ 255, 255, 255, 255 } : ska::Color{ 255, 215, 60, 255 });
	const auto screen = m_camera.getScreenSize();
	text(renderer, banner, 64, color, screen.x / 2, screen.y / 2 - 80, 0, scale, alpha);
}

const ska::Texture& GUIBattle::skillSprite(int id) const {
	auto found = m_skillSprites.find(id);
	if (found == m_skillSprites.end()) {
		found = m_skillSprites.emplace(id, ska::Texture{ "./Sprites/Fight/" + std::to_string(id) + ".png" }).first;
	}
	return found->second;
}

void GUIBattle::renderMarks(ska::Renderer& renderer) const {
	for (const auto& mark : m_stage.marks()) {
		const auto position = toScreen(mark.position);
		if (mark.kind == BattleMark::Kind::Trail) {
			// A faded, smaller copy of the projectile (its first frame)
			const auto& sheet = skillSprite(mark.value);
			const auto frameWidth = static_cast<int>(sheet.getWidth()) / 2;
			const auto frameHeight = static_cast<int>(sheet.getHeight()) / 2;
			const auto clip = ska::Rectangle{ 0, 0, frameWidth, frameHeight };
			const auto progress = static_cast<float>(mark.age) / 180.F;
			auto effect = ska::RenderEffect{};
			effect.anchorBottom = false;
			effect.scaleX = effect.scaleY = 0.9F - 0.5F * progress;
			effect.alpha = static_cast<unsigned char>(150.F * std::max(0.F, 1.F - progress));
			renderer.setEffect(&effect);
			renderer.render(sheet, position.x - frameWidth / 2, position.y - frameHeight / 2, &clip);
			renderer.setEffect(nullptr);
			continue;
		}
		if (mark.kind == BattleMark::Kind::Slash) {
			// The sprite of the skill where it hit, its 4 frames once, a little bigger
			const auto& sheet = skillSprite(mark.value);
			const auto frameWidth = static_cast<int>(sheet.getWidth()) / 2;
			const auto frameHeight = static_cast<int>(sheet.getHeight()) / 2;
			const auto frame = static_cast<int>(std::min(3u, mark.age * 4 / 240));
			const auto clip = ska::Rectangle{ (frame % 2) * frameWidth, (frame / 2) * frameHeight, frameWidth, frameHeight };
			auto effect = ska::RenderEffect{};
			effect.anchorBottom = false;
			effect.scaleX = effect.scaleY = 2.F;
			renderer.setEffect(&effect);
			renderer.render(sheet, position.x - frameWidth / 2, position.y - frameHeight / 2, &clip);
			renderer.setEffect(nullptr);
			continue;
		}
		if (mark.kind == BattleMark::Kind::Impact) {
			const auto progress = static_cast<float>(mark.age) / 220.F;
			auto effect = ska::RenderEffect{};
			effect.anchorBottom = false;
			effect.scaleX = effect.scaleY = (0.5F + 1.1F * progress) * (0.7F + static_cast<float>(mark.value) / 20.F);
			effect.alpha = static_cast<unsigned char>(255.F * std::max(0.F, 1.F - progress));
			renderer.setEffect(&effect);
			renderer.render(m_impact, position.x - static_cast<int>(m_impact.getWidth()) / 2, position.y - static_cast<int>(m_impact.getHeight()) / 2, nullptr);
			renderer.setEffect(nullptr);
			continue;
		}
		if (mark.kind == BattleMark::Kind::Text) {
			// A word (effectiveness, status, statistics) : it rises and fades
			const auto progress = std::min(1.F, static_cast<float>(mark.age) / 1100.F);
			const auto alpha = mark.age > 800 ? static_cast<unsigned char>(255 * (1100 - std::min(1100u, mark.age)) / 300) : static_cast<unsigned char>(255);
			const auto color = mark.value == 1 ? ska::Color{ 130, 230, 255, 255 } : (mark.value == 2 ? ska::Color{ 255, 140, 210, 255 } : ska::Color{ 255, 255, 255, 255 });
			const auto scale = mark.age < 100 ? 1.4F - 0.4F * static_cast<float>(mark.age) / 100.F : 1.F;
			text(renderer, mark.text, 16, color, position.x, position.y - static_cast<int>(24.F * progress), 0, scale, alpha);
			continue;
		}
		if (mark.kind == BattleMark::Kind::Wave) {
			// An area skill : a ring growing to its range
			const auto progress = std::min(1.F, static_cast<float>(mark.age) / 350.F);
			const auto radius = static_cast<float>(mark.value) * (0.25F + 0.75F * progress);
			const auto alpha = static_cast<uint8_t>(220.F * (1.F - progress));
			const auto color = mark.onPlayer ? ska::Color{ 120, 190, 255, alpha } : ska::Color{ 255, 140, 110, alpha };
			renderer.setRenderDrawBlendMode(SDL_BLENDMODE_BLEND);
			constexpr auto segments = 48;
			for (auto ring = 0; ring < 3; ring++) {
				const auto r = radius - static_cast<float>(ring);
				for (auto i = 0; i < segments; i++) {
					const auto a = 6.2832F * static_cast<float>(i) / segments;
					const auto b = 6.2832F * static_cast<float>(i + 1) / segments;
					renderer.drawColorLine(color, ska::Point<int>{ position.x + static_cast<int>(std::cos(a) * r), position.y + static_cast<int>(std::sin(a) * r * 0.6F) },
						ska::Point<int>{ position.x + static_cast<int>(std::cos(b) * r), position.y + static_cast<int>(std::sin(b) * r * 0.6F) });
				}
			}
			renderer.setRenderDrawBlendMode(SDL_BLENDMODE_NONE);
			continue;
		}
		// Damage : it pops, rises and fades
		const auto progress = static_cast<float>(mark.age) / 900.F;
		const auto rise = static_cast<int>(46.F * (1.F - (1.F - progress) * (1.F - progress)));
		const auto scale = mark.age < 120 ? 1.8F - 0.8F * static_cast<float>(mark.age) / 120.F : 1.F;
		const auto alpha = mark.age > 600 ? static_cast<unsigned char>(255 * (900 - std::min(900u, mark.age)) / 300) : static_cast<unsigned char>(255);
		const auto color = mark.onPlayer ? ska::Color{ 255, 100, 90, 255 } : ska::Color{ 255, 225, 80, 255 };
		text(renderer, mark.value > 0 ? std::to_string(mark.value) : std::string{ "0" }, mark.value >= 8 ? 34 : 28, color, position.x, position.y - 20 - rise, 0, scale, alpha);
	}
}

void GUIBattle::renderWorldOverlay(ska::Renderer& renderer) const {
	// The arrow over the Pokemon of the player
	if (visible(m_stage.player) && m_stage.phase() == BattlePhase::Fight) {
		const auto drawn = sprite(m_stage.player);
		const auto bob = static_cast<int>(3.F * std::sin(static_cast<float>(m_time) / 140.F));
		renderer.render(m_arrow, drawn.position.x + drawn.width / 2 - static_cast<int>(m_arrow.getWidth()) / 2, drawn.position.y - 16 + bob, nullptr);
	}
	// An enemy prepares an attack : "!" over it
	for (const auto entity : m_stage.fighters) {
		const auto* state = fighter(entity);
		if (state == nullptr || state->team == 0 || state->knockedOut || state->windUp == 0 || !visible(entity) || m_stage.phase() != BattlePhase::Fight) {
			continue;
		}
		const auto drawn = sprite(entity);
		const auto progress = state->windUpDuration == 0 ? 1.F : 1.F - static_cast<float>(state->windUp) / static_cast<float>(state->windUpDuration);
		text(renderer, "!", 30, ska::Color{ 255, static_cast<uint8_t>(220 - 160 * progress), 60, 255 }, drawn.position.x + drawn.width / 2, drawn.position.y - 34, 0, 0.8F + 0.5F * progress);
	}
	renderMarks(renderer);
}

void GUIBattle::renderOverlay(ska::Renderer& renderer) const {
	updateScreenShader(renderer);

	renderer.setRenderDrawBlendMode(SDL_BLENDMODE_BLEND);
	// The side of the player on the left, the wild Pokemon on the right
	std::array<int, 2> slots{ 0, 0 };
	for (const auto entity : m_stage.fighters) {
		const auto* state = fighter(entity);
		if (state == nullptr || state->team < 0 || state->team > 1) {
			continue;
		}
		renderPanel(renderer, entity, state->team == 0, slots[static_cast<std::size_t>(state->team)]++);
	}
	renderSkills(renderer);
	renderer.setRenderDrawBlendMode(SDL_BLENDMODE_NONE);
	const auto screen = m_camera.getScreenSize();
	text(renderer, "VS", 26, ska::Color{ 255, 255, 255, 255 }, screen.x / 2, 28, 0);
	renderBanner(renderer);
}
