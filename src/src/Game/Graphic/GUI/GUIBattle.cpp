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
#include "Canvas.h"

namespace {
	constexpr int GroundPriority = 9;

	// Upper case (UTF-8) : the ASCII letters and the accented Latin letters (à .. þ : 0xC3 0xA0 .. 0xC3 0xBE)
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



	// The texts of the battle : "outlined" in black over the map, else on a panel of the theme
	Ui::TextStyle Style(unsigned int size, ska::Color color, bool outlined = true, float scale = 1.F, unsigned char alpha = 255) {
		auto style = Ui::TextStyle{};
		style.size = size;
		style.color = color;
		style.outline = outlined ? 2 : 0;
		style.scale = scale;
		style.alpha = alpha;
		return style;
	}

	const ska::Color PlayerColor{ 60, 150, 255, 255 };
	const ska::Color OpponentColor{ 255, 70, 60, 255 };
	const auto& Ink = Ui::Ink;
	const auto& White = Ui::White;
}

GUIBattle::GUIBattle(ska::EntityManager& em, BattleStage& stage, const BattleSystem& battle, ska::CameraSystem& camera) :
	m_entities(em),
	m_stage(stage),
	m_battle(battle),
	m_camera(camera),
	m_groundLayer([this](Ui::Canvas& canvas) { renderGround(canvas.renderer()); }),
	m_worldOverlayLayer([this](Ui::Canvas& canvas) { renderWorldOverlay(canvas.renderer()); }),
	m_overlayLayer([this](Ui::Canvas& canvas) { renderOverlay(canvas.renderer()); }),
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
	const auto canvas = Ui::Canvas{ renderer };
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
	// (A capture : the colors stay)
	if (m_stage.phase() == BattlePhase::KnockOut && !(m_stage.winnerTeam == 0 && m_stage.captured)) {
		const auto progress = std::min(1.F, static_cast<float>(m_stage.phaseTime()) / 500.F);
		gray = 0.85F * progress;
		vignette = 0.8F * progress;
	} else if (m_stage.phase() == BattlePhase::Outro && m_stage.outro.fromKnockOut) {
		// The colors come back
		const auto progress = 1.F - std::min(1.F, static_cast<float>(m_stage.phaseTime()) / 600.F);
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
	const auto origin = graphic.spriteOrigin(std::lround(drawn.x), std::lround(drawn.y), hc);
	result.position.x = origin.x - camera.x;
	result.position.y = origin.y - camera.y - static_cast<int>(std::lround(drawn.z));
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


void GUIBattle::renderArena(ska::Renderer& renderer) const {
	const auto canvas = Ui::Canvas{ renderer };
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

	canvas.fill(floor, ska::Rectangle{ x, y, w, h });
	// Dashed edges : an overlay, not a wall
	constexpr auto dash = 18;
	constexpr auto gap = 12;
	for (auto i = 0; i < w; i += dash + gap) {
		canvas.fill(edge, ska::Rectangle{ x + i, y, std::min(dash, w - i), 2 });
		canvas.fill(edge, ska::Rectangle{ x + i, y + h - 2, std::min(dash, w - i), 2 });
	}
	for (auto i = 0; i < h; i += dash + gap) {
		canvas.fill(edge, ska::Rectangle{ x, y + i, 2, std::min(dash, h - i) });
		canvas.fill(edge, ska::Rectangle{ x + w - 2, y + i, 2, std::min(dash, h - i) });
	}
	// Corners
	constexpr auto corner = 30;
	constexpr auto thick = 4;
	canvas.fill(edge, ska::Rectangle{ x - 1, y - 1, corner, thick });
	canvas.fill(edge, ska::Rectangle{ x - 1, y - 1, thick, corner });
	canvas.fill(edge, ska::Rectangle{ x + w - corner + 1, y - 1, corner, thick });
	canvas.fill(edge, ska::Rectangle{ x + w - thick + 1, y - 1, thick, corner });
	canvas.fill(edge, ska::Rectangle{ x - 1, y + h - thick + 1, corner, thick });
	canvas.fill(edge, ska::Rectangle{ x - 1, y + h - corner + 1, thick, corner });
	canvas.fill(edge, ska::Rectangle{ x + w - corner + 1, y + h - thick + 1, corner, thick });
	canvas.fill(edge, ska::Rectangle{ x + w - thick + 1, y + h - corner + 1, thick, corner });
}

void GUIBattle::renderGround(ska::Renderer& renderer) const {
	const auto canvas = Ui::Canvas{ renderer };
	// The battle is over : the map again
	if (m_stage.phase() == BattlePhase::Outro) {
		return;
	}
	renderArena(renderer);

	// A ring under each fighter : blue for the player, red for the opponent
	for (const auto entity : m_stage.fighters) {
		const auto* state = fighter(entity);
		if (state == nullptr || !visible(entity) || state->knockedOut || state->inBall) {
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
	const auto canvas = Ui::Canvas{ renderer };
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
	canvas.panel("panel_small.png", ska::Rectangle{ x, y, width, height }, 16, 240);

	// Who it is : the tag (a pill of its side) and the name
	const auto tag = fighter.player ? std::string{ "VOUS" } : std::string{ "SAUVAGE" };
	const auto label = name(entity) + "  Nv." + std::to_string(bc.level) + (fighter.captured ? "  CAPTURÉ" : (fighter.knockedOut ? "  K.O." : ""));
	const auto tagWidth = fighter.player ? 62 : 92;
	const auto tagX = left ? x + 12 : x + width - 12 - tagWidth;
	canvas.panel(fighter.player ? "panel_small_teal.png" : "panel_small_red.png", ska::Rectangle{ tagX, y + 9, tagWidth, 24 }, 8);
	canvas.text(tag, Style(14, White, false), tagX + tagWidth / 2, y + 11, Ui::Align::Center);
	canvas.text(label, Style(18, Ink, false), left ? tagX + tagWidth + 10 : tagX - 10, y + 9, left ? Ui::Align::Left : Ui::Align::Right);

	// The hit points : a bar in the dark groove of the theme ; it empties towards the middle of the screen, the lost part
	// stays a moment
	const auto barX = x + 14;
	const auto barY = y + 40;
	const auto barWidth = width - 28;
	const auto barHeight = 18;
	const auto maxHp = static_cast<float>(std::max(1, bc.maxHp));
	const auto hp = std::max(0.F, static_cast<float>(bc.hp));
	const auto ratio = hp / maxHp;
	auto bar = Ui::Bar{};
	bar.ratio = ratio;
	bar.trail = std::max(hp, fighter.shownHp) / maxHp;
	bar.fill = ratio > 0.5F ? ska::Color{ 76, 200, 96, 255 } : (ratio > 0.25F ? ska::Color{ 240, 196, 52, 255 } : ska::Color{ 226, 64, 56, 255 });
	bar.fromRight = !left;
	canvas.bar(ska::Rectangle{ barX, barY, barWidth, barHeight }, bar);
	if (fighter.player) {
		canvas.text(std::to_string(std::max(0, bc.hp)) + " / " + std::to_string(bc.maxHp), Style(14, White), barX + barWidth - 6, barY + 1, Ui::Align::Right);
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
		canvas.text(tags, Style(13, ska::Color{ 168, 74, 24, 255 }, false), left ? barX : barX + barWidth, barY + barHeight + 6, left ? Ui::Align::Left : Ui::Align::Right);
	}

	// Fleeing : the time left outside of the arena
	if (fighter.player && fighter.outside > 0 && m_stage.phase() == BattlePhase::Fight) {
		const auto progress = std::min(1.F, static_cast<float>(fighter.outside) / static_cast<float>(BattleStage::FleeDelay));
		canvas.panel("panel_small_dark.png", ska::Rectangle{ x, y + height + 4, width / 2, 30 }, 8);
		canvas.fill(ska::Color{ 255, 90, 60, 255 }, ska::Rectangle{ x + 6, y + height + 26, static_cast<int>(static_cast<float>(width / 2 - 12) * progress), 4 });
		canvas.text("FUITE...", Style(16, White, false), x + 12, y + height + 7, Ui::Align::Left);
	}
}

void GUIBattle::renderSkills(ska::Renderer& renderer) const {
	const auto canvas = Ui::Canvas{ renderer };
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
	const auto rowX = x;
	auto y = screen.y - slotHeight - 14;
	// A slot of the theme : a panel, its key in a pill (teal : ready), the cooldown a shade shrinking over it
	auto slotPanel = [&](int sx, int width, int keyWidth, const std::string& key, const std::string& label, float cooldown) {
		const auto ready = cooldown <= 0.F;
		canvas.panel("panel_small.png", ska::Rectangle{ sx, y, width, slotHeight }, 16, 240);
		canvas.panel(ready ? "panel_small_teal.png" : "panel_small_dark.png", ska::Rectangle{ sx + 5, y + 5, keyWidth, slotHeight - 10 }, 8);
		canvas.text(key, Style(key.size() > 1 ? 15 : 20, White, false), sx + 5 + keyWidth / 2, y + (key.size() > 1 ? 13 : 10), Ui::Align::Center);
		canvas.text(label, Style(15, ready ? Ink : ska::Color{ 130, 134, 144, 255 }, false), sx + keyWidth + 14, y + 13, Ui::Align::Left);
		canvas.fill(ska::Color{ 34, 40, 54, 90 }, ska::Rectangle{ sx + keyWidth + 8, y + 5, static_cast<int>(static_cast<float>(width - keyWidth - 14) * cooldown), slotHeight - 10 });
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
	// The item of the trainer (TrainerItemSystem), over the skills : E (the "Obj." of the pad) uses it, R (the "Suiv." of
	// the pad) chooses the next one
	const auto& item = m_stage.itemSlot;
	if (item.item >= 0) {
		y -= slotHeight + 10;
		const auto label = Upper(item.name) + " x" + std::to_string(item.amount);
		slotPanel(rowX, 214, 30, "E", label, item.cooldown);
		canvas.text("R : OBJET SUIVANT", Style(13, White, true), rowX + 224, y + 14, Ui::Align::Left);
	}
}

void GUIBattle::renderBanner(ska::Renderer& renderer) const {
	const auto canvas = Ui::Canvas{ renderer };
	const auto& banner = m_stage.banner();
	if (banner.empty()) {
		return;
	}
	const auto age = m_stage.bannerAge();
	const auto lasting = m_stage.phase() == BattlePhase::KnockOut || m_stage.phase() == BattlePhase::Over;
	if (!lasting && age > 1200 && m_stage.phase() != BattlePhase::Outro) {
		return;
	}
	// The outro : the banner of the end goes away
	if (m_stage.phase() == BattlePhase::Outro && m_stage.phaseTime() >= 300) {
		return;
	}
	// It hits the screen (big, then normal), then fades
	const auto scale = age < 160 ? 2.2F - 1.2F * static_cast<float>(age) / 160.F : 1.F;
	auto alpha = !lasting && age > 900 && m_stage.phase() != BattlePhase::Outro ? static_cast<unsigned char>(255 * (1200 - std::min(1200u, age)) / 300) : static_cast<unsigned char>(255);
	if (m_stage.phase() == BattlePhase::Outro) {
		alpha = static_cast<unsigned char>(255 * (300 - m_stage.phaseTime()) / 300);
	}
	const auto color = banner == "K.O. !" ? ska::Color{ 255, 70, 50, 255 } : (banner == "FUITE !" ? ska::Color{ 255, 255, 255, 255 } : ska::Color{ 255, 215, 60, 255 });
	const auto screen = m_camera.getScreenSize();
	canvas.text(banner, Style(64, color, true, scale, alpha), screen.x / 2, screen.y / 2 - 80, Ui::Align::Center);
}

const ska::Texture& GUIBattle::skillSprite(int id) const {
	auto found = m_skillSprites.find(id);
	if (found == m_skillSprites.end()) {
		found = m_skillSprites.emplace(id, ska::Texture{ "./Sprites/Fight/" + std::to_string(id) + ".png" }).first;
	}
	return found->second;
}

void GUIBattle::renderMarks(ska::Renderer& renderer) const {
	const auto canvas = Ui::Canvas{ renderer };
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
			canvas.text(mark.text, Style(16, color, true, scale, alpha), position.x, position.y - static_cast<int>(24.F * progress), Ui::Align::Center);
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
		canvas.text(mark.value > 0 ? std::to_string(mark.value) : std::string{ "0" }, Style(mark.value >= 8 ? 34 : 28, color, true, scale, alpha), position.x, position.y - 20 - rise, Ui::Align::Center);
	}
}

void GUIBattle::renderWorldOverlay(ska::Renderer& renderer) const {
	const auto canvas = Ui::Canvas{ renderer };
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
		canvas.text("!", Style(30, ska::Color{ 255, static_cast<uint8_t>(220 - 160 * progress), 60, 255 }, true, 0.8F + 0.5F * progress, 255), drawn.position.x + drawn.width / 2, drawn.position.y - 34, Ui::Align::Center);
	}
	renderMarks(renderer);
}

void GUIBattle::renderOutro(ska::Renderer& renderer) const {
	const auto canvas = Ui::Canvas{ renderer };
	const auto& outro = m_stage.outro;
	if (outro.experience <= 0 && outro.levelAfter == outro.levelBefore) {
		return;
	}
	const auto time = m_stage.phaseTime();
	if (time < OutroCardDelay) {
		return;
	}
	// The card slides in from the top, the bar fills (towards the next level, again for each level won)
	const auto screen = m_camera.getScreenSize();
	const auto width = 460;
	const auto height = 92;
	const auto slide = std::min(1.F, static_cast<float>(time - OutroCardDelay) / 260.F);
	const auto x = screen.x / 2 - width / 2;
	const auto y = static_cast<int>(-static_cast<float>(height) + (14.F + static_cast<float>(height)) * (1.F - (1.F - slide) * (1.F - slide)));
	canvas.panel("panel_small.png", ska::Rectangle{ x, y, width, height }, 16, 240);

	const auto fill = std::clamp(static_cast<float>(static_cast<int>(time) - static_cast<int>(OutroBarDelay)) / static_cast<float>(OutroBarDuration), 0.F, 1.F);
	const auto eased = 1.F - (1.F - fill) * (1.F - fill);
	const auto before = static_cast<float>(outro.levelBefore) + outro.progressBefore;
	const auto after = static_cast<float>(outro.levelAfter) + outro.progressAfter;
	const auto value = before + (after - before) * eased;
	auto level = std::min(outro.levelAfter, static_cast<int>(std::floor(value)));
	auto ratio = value - static_cast<float>(level);
	if (level == outro.levelAfter && fill >= 1.F) {
		ratio = outro.progressAfter;
	}

	canvas.panel("panel_small_teal.png", ska::Rectangle{ x + 12, y + 9, 62, 24 }, 8);
	canvas.text("EXP", Style(14, White, false), x + 12 + 31, y + 11, Ui::Align::Center);
	canvas.text(Upper(outro.pokemon) + "  Nv." + std::to_string(level), Style(18, Ink, false), x + 84, y + 9, Ui::Align::Left);
	if (outro.experience > 0) {
		canvas.text("+" + std::to_string(outro.experience) + " EXP", Style(18, ska::Color{ 196, 132, 20, 255 }, false), x + width - 14, y + 9, Ui::Align::Right);
	}
	auto bar = Ui::Bar{};
	bar.ratio = std::clamp(ratio, 0.F, 1.F);
	bar.trail = bar.ratio;
	bar.fill = ska::Color{ 80, 160, 255, 255 };
	canvas.bar(ska::Rectangle{ x + 14, y + 44, width - 28, 16 }, bar);

	// A level won : it pops over the card
	if (level > outro.levelBefore) {
		const auto levelUpAt = OutroBarDelay + static_cast<unsigned int>(static_cast<float>(OutroBarDuration) * std::clamp((static_cast<float>(outro.levelBefore + 1) - before) / std::max(0.001F, after - before), 0.F, 1.F));
		const auto age = time > levelUpAt ? time - levelUpAt : 0u;
		const auto scale = age < 160 ? 1.8F - 0.8F * static_cast<float>(age) / 160.F : 1.F + 0.04F * std::sin(static_cast<float>(m_time) / 90.F);
		canvas.text("NIVEAU SUPÉRIEUR !", Style(30, ska::Color{ 255, 215, 60, 255 }, true, scale), screen.x / 2, y + height + 10, Ui::Align::Center);
	}
}

void GUIBattle::renderBond(ska::Renderer& renderer) const {
	const auto* player = fighter(m_stage.player);
	if (player == nullptr || player->knockedOut) {
		return;
	}
	const auto canvas = Ui::Canvas{ renderer };
	// Under the panel of the Pokemon of the player : the gauge of the bond with its trainer
	const auto x = 14 + 12;
	const auto y = 14 + 84 + 8;
	const auto full = m_stage.bond >= BattleStage::BondFull;
	const auto pulse = 0.5F + 0.5F * std::sin(static_cast<float>(m_time) / 110.F);
	canvas.panel(full ? "panel_small_red.png" : "panel_small_teal.png", ska::Rectangle{ x, y, 62, 24 }, 8, 230);
	canvas.text("LIEN", Style(14, White, false), x + 31, y + 2, Ui::Align::Center);
	auto bar = Ui::Bar{};
	bar.ratio = static_cast<float>(m_stage.bond) / static_cast<float>(BattleStage::BondFull);
	bar.trail = bar.ratio;
	bar.fill = full ? ska::Color{ 255, static_cast<uint8_t>(150 + 90 * pulse), 60, 255 } : ska::Color{ 255, 180, 70, 255 };
	canvas.bar(ska::Rectangle{ x + 76, y + 7, 170, 10 }, bar);
	if (full && !m_stage.cinematic.active) {
		canvas.text("F : LIEN !", Style(16, ska::Color{ 255, 230, 120, 255 }, true, 1.F + 0.08F * pulse), x + 262, y + 1, Ui::Align::Left);
	}
}

void GUIBattle::renderCinematic(ska::Renderer& renderer) const {
	const auto& cinematic = m_stage.cinematic;
	if (!cinematic.active) {
		return;
	}
	const auto canvas = Ui::Canvas{ renderer };
	const auto screen = m_camera.getScreenSize();
	const auto t = static_cast<float>(cinematic.time);
	const auto ease = [](float v) { v = std::clamp(v, 0.F, 1.F); return 1.F - (1.F - v) * (1.F - v); };
	// Everything comes in, then goes away once the skill went
	const auto in = ease(t / 180.F);
	const auto out = std::clamp((static_cast<float>(BondCinematic::Duration) - t) / 300.F, 0.F, 1.F);
	const auto shown = std::min(in, out);

	// The world darker, black bars
	canvas.fill(ska::Color{ 0, 0, 0, static_cast<uint8_t>(80.F * shown) }, ska::Rectangle{ 0, 0, screen.x, screen.y });
	const auto bars = static_cast<int>(70.F * shown);
	canvas.fill(ska::Color{ 0, 0, 0, 255 }, ska::Rectangle{ 0, 0, screen.x, bars });
	canvas.fill(ska::Color{ 0, 0, 0, 255 }, ska::Rectangle{ 0, screen.y - bars, screen.x, bars });

	// The cut-in : a band slides in, the trainer calls the skill
	const auto bandIn = ease((t - 120.F) / 260.F);
	const auto bandOut = std::clamp((static_cast<float>(BondCinematic::Release) + 150.F - t) / 200.F, 0.F, 1.F);
	if (bandIn > 0.F && bandOut > 0.F) {
		const auto bandHeight = 120;
		const auto bandY = screen.y - bars - bandHeight - 30;
		const auto slide = static_cast<int>(static_cast<float>(screen.x) * (1.F - bandIn)) - static_cast<int>(static_cast<float>(screen.x) * (1.F - bandOut));
		const auto alpha = static_cast<uint8_t>(225.F * std::min(bandIn, bandOut));
		const ska::Color gold{ 255, 200, 70, alpha };
		canvas.fill(ska::Color{ 18, 22, 46, alpha }, ska::Rectangle{ -slide, bandY, screen.x, bandHeight });
		canvas.fill(gold, ska::Rectangle{ -slide, bandY, screen.x, 3 });
		canvas.fill(gold, ska::Rectangle{ -slide, bandY + bandHeight - 3, screen.x, 3 });
		// The trainer, big (its sprite as the map draws it : it faces the camera)
		if (m_stage.trainer.has_value() && m_entities.hasComponent<ska::GraphicComponent>(*m_stage.trainer)) {
			const auto& graphic = m_entities.getComponent<ska::GraphicComponent>(*m_stage.trainer);
			if (!graphic.animatedSprites.empty()) {
				const auto& sprite = graphic.animatedSprites[0];
				auto effect = ska::RenderEffect{};
				effect.scaleX = 2.6F;
				effect.scaleY = 2.6F;
				effect.anchorBottom = true;
				effect.alpha = alpha;
				renderer.setEffect(&effect);
				sprite.render(renderer, 90 - slide - static_cast<int>(sprite.getWidth()) / 2, bandY + bandHeight - 6 - static_cast<int>(sprite.getHeight()), nullptr, 0., nullptr);
				renderer.setEffect(nullptr);
			}
		}
		canvas.text(cinematic.line, Style(22, White, true, 1.F, alpha), 170 - slide, bandY + 18, Ui::Align::Left);
		canvas.text(Upper(cinematic.skill), Style(40, gold, true, 1.F, alpha), 170 - slide, bandY + 52, Ui::Align::Left);
	}

	// The ring closes on the Pokemon : F when it reaches the inner ring
	if (visible(cinematic.user) && t >= static_cast<float>(BondCinematic::RingStart) && !cinematic.released) {
		const auto drawn = sprite(cinematic.user);
		const auto zoom = m_camera.getZoom();
		const auto cx = static_cast<float>(drawn.position.x + drawn.width / 2) * zoom;
		const auto cy = static_cast<float>(drawn.position.y + drawn.height / 2) * zoom;
		const auto progress = (t - static_cast<float>(BondCinematic::RingStart)) / static_cast<float>(BondCinematic::RingClose - BondCinematic::RingStart);
		const auto inner = 42.F;
		const auto circle = [&](float radius, ska::Color color, int dot) {
			constexpr auto Dots = 56;
			for (auto i = 0; i < Dots; i++) {
				const auto angle = static_cast<float>(i) * 6.2831853F / static_cast<float>(Dots);
				canvas.fill(color, ska::Rectangle{ static_cast<int>(cx + std::cos(angle) * radius) - dot / 2, static_cast<int>(cy + std::sin(angle) * radius) - dot / 2, dot, dot });
			}
		};
		circle(inner, ska::Color{ 255, 255, 255, 170 }, 4);
		if (!cinematic.pressed) {
			const auto radius = std::max(inner * 0.5F, inner + (190.F - inner) * (1.F - progress));
			circle(radius, ska::Color{ 255, 200, 70, 255 }, 6);
		} else {
			const auto age = t - static_cast<float>(cinematic.pressedAt);
			const auto fade = static_cast<uint8_t>(255.F * std::clamp(1.F - age / 300.F, 0.F, 1.F));
			const auto color = cinematic.perfect ? ska::Color{ 255, 230, 90, fade } : ska::Color{ 200, 200, 220, fade };
			circle(inner + age * 0.15F, color, 6);
			canvas.text(cinematic.perfect ? "PARFAIT !" : "BIEN", Style(30, color, true, 1.F, fade), static_cast<int>(cx), static_cast<int>(cy - inner - 56.F), Ui::Align::Center);
		}
	}

	// The skill goes : a flash
	if (cinematic.released) {
		const auto age = t - static_cast<float>(BondCinematic::Release);
		const auto flash = std::clamp(1.F - age / 220.F, 0.F, 1.F);
		if (flash > 0.F) {
			canvas.fill(ska::Color{ 255, 255, 255, static_cast<uint8_t>(200.F * flash) }, ska::Rectangle{ 0, 0, screen.x, screen.y });
		}
	}
}

void GUIBattle::renderSpeech(ska::Renderer& renderer) const {
	const auto& speech = m_stage.speech();
	if (speech.empty() || !m_stage.trainer.has_value() || !visible(*m_stage.trainer)) {
		return;
	}
	const auto canvas = Ui::Canvas{ renderer };
	// Over the head of the trainer : the world is drawn zoomed, the interface is not
	const auto drawn = sprite(*m_stage.trainer);
	const auto zoom = m_camera.getZoom();
	const auto headX = static_cast<int>(std::lround(static_cast<float>(drawn.position.x + drawn.width / 2) * zoom));
	const auto headY = static_cast<int>(std::lround(static_cast<float>(drawn.position.y) * zoom));
	// It pops in (it rises a little), it fades out
	const auto age = m_stage.speechAge();
	const auto duration = std::max(1U, m_stage.speechDuration());
	const auto in = std::min(1.F, static_cast<float>(age) / 120.F);
	const auto out = std::min(1.F, static_cast<float>(duration - std::min(age, duration)) / 250.F);
	const auto alpha = static_cast<unsigned char>(255.F * std::min(in, out));
	const auto rise = static_cast<int>(std::lround(6.F * (1.F - in)));

	constexpr auto TextSize = 18U;
	auto measure = ska::Texture{};
	measure.loadFromText(TextSize, speech, Ink);
	const auto textWidth = static_cast<int>(measure.getWidth());
	const auto textHeight = static_cast<int>(measure.getHeight());
	// The tail of the balloon (Menu/default_theme/balloon.png, its bottom left) points at the head
	constexpr auto Tail = 16;
	const auto width = std::max(60, textWidth + 28);
	const auto height = std::max(48, textHeight + 26);
	const auto screen = m_camera.getScreenSize();
	const auto x = std::clamp(headX - Tail, 4, std::max(4, screen.x - width - 4));
	const auto y = std::max(4, headY - height - 2 + rise);
	canvas.panel("balloon.png", ska::Rectangle{ x, y, width, height }, 24, alpha);
	canvas.text(speech, Style(TextSize, Ink, false, 1.F, alpha), x + width / 2, y + (height - 6 - textHeight) / 2, Ui::Align::Center);
}

void GUIBattle::renderOverlay(ska::Renderer& renderer) const {
	const auto canvas = Ui::Canvas{ renderer };
	updateScreenShader(renderer);
	if (m_stage.phase() == BattlePhase::Outro) {
		renderOutro(renderer);
		renderBanner(renderer);
		return;
	}

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
	renderBond(renderer);
	renderSpeech(renderer);
	const auto screen = m_camera.getScreenSize();
	canvas.text("VS", Style(26, ska::Color{ 255, 255, 255, 255 }), screen.x / 2, 28, Ui::Align::Center);
	renderBanner(renderer);
	renderCinematic(renderer);
}
