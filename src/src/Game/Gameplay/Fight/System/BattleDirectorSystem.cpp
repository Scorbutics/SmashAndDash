#include "Game/__internalConfig/LoggerConfig.h"
#include "BattleDirectorSystem.h"
#include <algorithm>
#include <array>
#include "Base/IO/Readers/IniReader.h"
#include "Core/ECS/EntityLocator.h"
#include "Graphic/System/CameraSystem.h"
#include "Graphic/System/CameraFixedStrategy.h"
#include "../../Control/ExploreCameraStrategy.h"
#include "../BattleRules.h"
#include "../../Data/MapEvent.h"
#include "../../../Script/ScriptConstants.h"
#include "Game/Gameplay/Data/SavePaths.h"
#include "Scripting/ScriptVM.h"
#include "../../Data/Team.h"
#include "../../PokeballComponent.h"
#include "../../System/PokeballSystem.h"
#include "../../CustomEntityManager.h"
#include "Core/ECS/Basics/Graphic/AnimationComponent.h"

BattleDirectorSystem::BattleDirectorSystem(ska::EntityManager& em, WorldState& ws, PokemonGameEventDispatcher& ged, BattleStage& stage, int teamIndex) :
	System(em),
	m_entities(em),
	m_worldState(ws),
	m_ged(ged),
	m_stage(stage),
	m_teamIndex(teamIndex) {
	m_stage.setCamera(&m_worldState.getCamera());
}

bool BattleDirectorSystem::present(ska::EntityId fighter) const {
	const auto& processed = getEntities();
	return processed.find(fighter) != processed.end();
}

ska::Point<float> BattleDirectorSystem::center(ska::EntityId fighter) {
	return ska::PositionComponent::getCenterPosition(m_componentAccessor.get<ska::PositionComponent>(fighter), m_componentAccessor.get<ska::HitboxComponent>(fighter));
}

void BattleDirectorSystem::start() {
	// The arena : around the fighters, in the map
	const auto blockSize = static_cast<int>(m_worldState.getWorld().getBlockSize());
	auto a = center(m_stage.player);
	auto b = a;
	for (const auto entity : m_stage.fighters) {
		const auto position = center(entity);
		a = ska::Point<float>{ std::min(a.x, position.x), std::min(a.y, position.y) };
		b = ska::Point<float>{ std::max(b.x, position.x), std::max(b.y, position.y) };
	}
	const auto width = ArenaWidth * blockSize;
	const auto height = ArenaHeight * blockSize;
	const auto worldWidth = static_cast<int>(m_worldState.getWorld().getPixelWidth());
	const auto worldHeight = static_cast<int>(m_worldState.getWorld().getPixelHeight());
	auto x = static_cast<int>((a.x + b.x) / 2.F) - width / 2;
	auto y = static_cast<int>((a.y + b.y) / 2.F) - height / 2;
	x = std::max(0, std::min(x, worldWidth - width));
	y = std::max(0, std::min(y, worldHeight - height));
	m_stage.arena = ska::Rectangle{ x, y, width, height };

	// The camera of the battle : BattleCameraSystem

	for (const auto entity : getEntities()) {
		m_componentAccessor.get<FighterComponent>(entity).shownHp = static_cast<float>(m_componentAccessor.get<BattleComponent>(entity).hp);
	}
	m_stage.changePhase(BattlePhase::Starting);
	const auto opponent = m_stage.fighters.size() > 1 ? m_stage.fighters[1] : m_stage.player;
	runStep({ "start", name(m_stage.player), name(opponent), std::to_string(m_componentAccessor.get<BattleComponent>(opponent).level) });
	SLOG(ska::LogLevel::Info) << "Battle : fight in the arena " << x << ", " << y << " (" << width << " x " << height << ")";
}

void BattleDirectorSystem::runStep(std::vector<std::string> arguments) {
	auto done = std::make_shared<bool>(false);
	m_stepDone = done;
	auto context = ska::scripting::ScriptContext{};
	context.name = StepsFile;
	const auto step = arguments.empty() ? std::string{} : arguments[0];
	SLOG(ska::LogLevel::Info) << "Battle : step " << step;
	const auto started = m_worldState.getScripts().start(StepsFile, std::move(arguments), context, [done, step](bool ok, const std::string& text) {
		if (!ok) {
			SLOG_STATIC(ska::LogLevel::Error, BattleDirectorSystem) << "Battle : the step " << step << " failed : " << text;
		}
		*done = true;
	});
	if (!started) {
		*done = true;
	}
}

bool BattleDirectorSystem::stepDone() const {
	if (m_stepDone == nullptr || *m_stepDone) {
		return true;
	}
	// The dialogs of the outro wait for the player
	if (m_stage.phase() != BattlePhase::Outro && m_stage.phaseTime() >= StepTimeout) {
		SLOG(ska::LogLevel::Warn) << "Battle : the step script runs for too long, the battle goes on";
		*m_stepDone = true;
	}
	return *m_stepDone;
}

std::string BattleDirectorSystem::name(ska::EntityId fighter) const {
	const auto& bc = m_componentAccessor.get<BattleComponent>(fighter);
	const auto data = ska::IniReader{ "./Data/Monsters/" + std::to_string(bc.monster) + ".ini" };
	return data.exists("Description name") ? data.get<std::string>("Description name") : std::string{ "?" };
}

void BattleDirectorSystem::updateBars(unsigned int ellapsedTime) {
	for (const auto entity : getEntities()) {
		auto* fighter = &m_componentAccessor.get<FighterComponent>(entity);
		const auto& bc = m_componentAccessor.get<BattleComponent>(entity);
		const auto hp = static_cast<float>(std::max(0, bc.hp));
		fighter->barShake = fighter->barShake > ellapsedTime ? fighter->barShake - ellapsedTime : 0;
		if (fighter->shownHp <= hp) {
			fighter->shownHp = hp;
			continue;
		}
		// The lost part stays a moment, then drains
		if (fighter->hpDelay > ellapsedTime) {
			fighter->hpDelay -= ellapsedTime;
			continue;
		}
		fighter->hpDelay = 0;
		const auto drain = std::max(1.F, static_cast<float>(bc.maxHp) * 0.9F) * static_cast<float>(ellapsedTime) / 1000.F;
		fighter->shownHp = std::max(hp, fighter->shownHp - drain);
	}
}

void BattleDirectorSystem::refresh(unsigned int ellapsedTime) {
	// The scripts go on during the battle (the map, that updates them, is not running) : its steps wait
	m_worldState.getScripts().update(ellapsedTime);
	m_stage.update(ellapsedTime);
	updateBars(ellapsedTime);

	switch (m_stage.phase()) {
	case BattlePhase::Intro:
		if (std::all_of(m_stage.fighters.begin(), m_stage.fighters.end(), [this](ska::EntityId entity) { return present(entity); })) {
			start();
		}
		break;

	case BattlePhase::Starting:
		if (stepDone()) {
			m_stage.changePhase(BattlePhase::Fight);
		}
		break;

	case BattlePhase::Fight:
		{
			// A fighter without hit points is K.O. : it leaves the fight. A side without fighter standing lost
			std::array<int, 2> standing{ 0, 0 };
			for (const auto entity : getEntities()) {
				auto& fighter = m_componentAccessor.get<FighterComponent>(entity);
				if (!fighter.knockedOut && m_componentAccessor.get<BattleComponent>(entity).hp <= 0) {
					fighter.knockedOut = true;
					fighter.knockedOutTime = 0;
					// The last one out of the fight is K.O., not captured
					m_stage.captured = false;
					m_stage.hitStop(180);
					m_stage.shake(260, 7);
					m_stage.sound(BattleSound::KnockOut);
					SLOG(ska::LogLevel::Info) << "Battle : " << (fighter.player ? "the pokemon of the player" : "a wild pokemon") << " is K.O.";
				}
				if (!fighter.knockedOut && fighter.team >= 0 && fighter.team < 2) {
					standing[static_cast<std::size_t>(fighter.team)]++;
				}
			}
			if (standing[0] == 0 || standing[1] == 0) {
				m_stage.winnerTeam = standing[0] == 0 ? 1 : 0;
				m_stage.changePhase(BattlePhase::KnockOut);
				const auto opponent = m_stage.fighters.size() > 1 ? m_stage.fighters[1] : m_stage.player;
				const auto byCapture = m_stage.winnerTeam == 0 && m_stage.captured;
				// A K.O. : a hit stop, a slow motion, a shake (a capture : none, the ball clicked)
				if (!byCapture) {
					m_stage.hitStop(250);
					m_stage.slowMotion(0.25F, 1400);
					m_stage.shake(400, 10);
				}
				runStep({ "knockout", byCapture ? "captured" : (m_stage.winnerTeam == 0 ? "won" : "lost"), name(m_stage.player), byCapture ? m_stage.capturedName : name(opponent) });
				SLOG(ska::LogLevel::Info) << "Battle : the side " << m_stage.winnerTeam << " won";
			}
		}
		if (m_stage.phase() == BattlePhase::Fight && m_stage.canFlee && present(m_stage.player) && m_componentAccessor.get<FighterComponent>(m_stage.player).outside >= BattleStage::FleeDelay) {
			m_stage.changePhase(BattlePhase::Fled);
			runStep({ "fled", name(m_stage.player) });
			SLOG(ska::LogLevel::Info) << "Battle : the player flees";
		}
		break;

	case BattlePhase::KnockOut:
		if (m_stage.phaseTime() >= KnockOutDuration && stepDone()) {
			outro();
		}
		break;

	case BattlePhase::Fled:
		if (m_stage.phaseTime() >= FleeDuration && stepDone()) {
			outro();
		}
		break;

	case BattlePhase::Outro:
		updateOutro();
		break;

	default:
		break;
	}
}

void BattleDirectorSystem::outro() {
	const auto fled = m_stage.phase() == BattlePhase::Fled;
	const auto won = !fled && m_stage.winnerTeam == 0;
	m_stage.outro = BattleOutro{};
	// The screen was gray (a K.O., not a capture)
	m_stage.outro.fromKnockOut = !fled && !(won && m_stage.captured);
	m_stage.changePhase(BattlePhase::Outro);
	SLOG(ska::LogLevel::Info) << "Battle " << (fled ? "fled" : (won ? "won" : "lost")) << " : the outro";

	// Result : hit points left of the Pokemon of the player, experience won for each wild Pokemon K.O. (battle rules)
	m_result = BattleResult{};
	m_result.won = won;
	m_result.fled = fled;
	m_result.teamIndex = m_teamIndex;
	const auto& pokemon = m_componentAccessor.get<BattleComponent>(m_stage.player);
	m_result.hpLeft = std::max(0, pokemon.hp);
	// The rewards (Data/Battle/rewards.miniska) : for the wild Pokemon K.O.
	auto dropped = std::vector<int>{};
	auto levels = std::vector<int>{};
	for (const auto entity : m_stage.fighters) {
		if (!present(entity) || m_componentAccessor.get<FighterComponent>(entity).team != 1 || !m_componentAccessor.get<FighterComponent>(entity).knockedOut) {
			continue;
		}
		const auto& opponent = m_componentAccessor.get<BattleComponent>(entity);
		const auto data = ska::IniReader{ "./Data/Monsters/" + std::to_string(opponent.monster) + ".ini" };
		dropped.push_back(data.exists("Experience level_1_exp_dropped") ? data.get<int>("Experience level_1_exp_dropped") : 0);
		levels.push_back(static_cast<int>(opponent.level));
	}
	auto& rules = m_worldState.getBattleRules();
	m_result.experience = rules.rewardExperience(dropped, levels, won, fled);

	// The experience bar of the outro : the level and its progress, before and after
	m_stage.outro.pokemon = name(m_stage.player);
	m_stage.outro.experience = m_result.experience;
	m_stage.outro.levelBefore = m_stage.outro.levelAfter = static_cast<int>(pokemon.level);
	if (m_teamIndex >= 0) {
		const auto member = Team{ m_worldState.getSaveGame().getPathName() }.member(m_teamIndex);
		if (member.has_value()) {
			const auto data = ska::IniReader{ "./Data/Monsters/" + std::to_string(member->id) + ".ini" };
			const auto needed = data.exists("Experience level_1_exp_needed") ? data.get<int>("Experience level_1_exp_needed") : 0;
			const auto progress = [&rules, needed](int experience, int& level) {
				level = rules.level(experience, needed);
				const auto start = rules.levelExperience(level, needed);
				const auto next = rules.levelExperience(level + 1, needed);
				return next > start ? std::clamp(static_cast<float>(experience - start) / static_cast<float>(next - start), 0.F, 1.F) : 1.F;
			};
			m_stage.outro.progressBefore = progress(member->experience, m_stage.outro.levelBefore);
			m_stage.outro.progressAfter = progress(member->experience + m_result.experience, m_stage.outro.levelAfter);
		}
	}

	// The Pokemon of the player goes back in its pokeball (K.O. : it already disappeared, the ball comes back at once)
	auto& fighter = m_componentAccessor.get<FighterComponent>(m_stage.player);
	fighter.recall = fighter.knockedOut ? FighterComponent::RecallDuration : 0;
	m_recallBall.reset();
	m_recallThrown = false;
	m_stage.sound(BattleSound::Cast);

	const auto opponent = m_stage.fighters.size() > 1 ? m_stage.fighters[1] : m_stage.player;
	const auto byCapture = won && m_stage.captured;
	runStep({ "outro", fled ? "fled" : (byCapture ? "captured" : (won ? "won" : "lost")), name(m_stage.player), byCapture ? m_stage.capturedName : name(opponent), std::to_string(m_result.experience),
		std::to_string(m_stage.outro.levelBefore), std::to_string(m_stage.outro.levelAfter) });
}

void BattleDirectorSystem::updateOutro() {
	auto& fighter = m_componentAccessor.get<FighterComponent>(m_stage.player);
	const auto* trainer = m_stage.trainer.has_value() && m_entities.hasComponent<ska::PositionComponent>(*m_stage.trainer) && m_entities.hasComponent<ska::HitboxComponent>(*m_stage.trainer) ?
		&*m_stage.trainer : nullptr;
	// Recalled : the ball flies from it back to the trainer (who faces it)
	if (!m_recallThrown && fighter.recall >= FighterComponent::RecallDuration) {
		m_recallThrown = true;
		m_recallThrownAt = m_stage.phaseTime();
		if (trainer != nullptr) {
			const auto from = center(m_stage.player);
			const auto to = ska::PositionComponent::getCenterPosition(m_entities.getComponent<ska::PositionComponent>(*trainer), m_entities.getComponent<ska::HitboxComponent>(*trainer));
			m_recallBall = PokeballSystem::Throw(m_entities, from, to, true);
			if (m_entities.hasComponent<ska::AnimationComponent>(*trainer)) {
				const auto dx = from.x - to.x;
				const auto dy = from.y - to.y;
				m_entities.getComponent<ska::AnimationComponent>(*trainer).state = std::abs(dx) > std::abs(dy) ? (dx > 0.F ? 1 : 3) : (dy > 0.F ? 0 : 2);
			}
		}
	}
	const auto ballBack = m_recallThrown && (!m_recallBall.has_value() || !m_entities.hasComponent<PokeballComponent>(*m_recallBall) ||
		m_stage.phaseTime() - m_recallThrownAt >= RecallBallTimeout);
	if (ballBack && m_stage.phaseTime() >= OutroDuration && stepDone()) {
		end();
	}
}

void BattleDirectorSystem::end() {
	m_stage.changePhase(BattlePhase::Over);
	SLOG(ska::LogLevel::Info) << "Battle " << (m_result.fled ? "fled" : (m_result.won ? "won" : "lost"));

	// The wild Pokemon of the map is defeated (the others of its group only exist during the battle : StateFight). Its
	// physics body goes away (the map is not loaded again)
	// Captured : it is in the team now (the battle can still be lost or fled after)
	const auto capturedOnMap = m_stage.fighters.size() > 1 && present(m_stage.fighters[1]) && m_componentAccessor.get<FighterComponent>(m_stage.fighters[1]).captured;
	if ((m_result.won || capturedOnMap) && m_stage.fighters.size() > 1) {
		CustomEntityManager::place(m_entities, m_worldState.getSpace(), m_stage.fighters[1], ska::Point<float>{ -100000.F, -100000.F });
		scheduleDeferredRemove(m_stage.fighters[1]);
	}
	m_worldState.battleEnded(m_result);
	// The next battle : easy, unless a script asks otherwise (Battle.difficulty)
	m_worldState.getBattleRules().setDifficulty(BattleRules::Easy);

	// Back to the map, as it is : no fade, no loading (the camera already went back to the trainer)
	MapEvent me(MapEventType::Switch_Map);
	me.mapName = m_worldState.getMapName();
	const auto mapReader = ska::IniReader{ MapIni(me.mapName) };
	me.chipsetName = mapReader.get<std::string>("Chipset file");
	const auto* trainer = m_worldState.getEntityLocator().getEntityId(SCRIPT_ENTITY_TRAINER);
	const auto& pc = m_entities.getComponent<ska::PositionComponent>(trainer != nullptr && m_entities.hasComponent<ska::PositionComponent>(*trainer) ? *trainer : m_stage.player);
	me.position = pc;
	me.resume = true;
	m_worldState.getCamera().changeStrategy(std::make_unique<ExploreCameraStrategy>(m_entities));
	m_worldState.getCamera().setZoom(WorldState::MapZoom);
	m_ged.ska::Observable<MapEvent>::notifyObservers(me);
}
