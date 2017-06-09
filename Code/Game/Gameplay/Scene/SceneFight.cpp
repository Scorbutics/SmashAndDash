#include "Core/Window.h"
#include "../World/WorldScene.h"
#include "SceneFight.h"
#include "SceneGUIBattle.h"
#include "Task/RepeatableTask.h"
#include "../CustomEntityManager.h"
#include "../Fight/FightComponent.h"
#include "../Data/PokemonDescriptor.h"
#include "Task/TaskQueue.h"

#include "../PokemonGameEventDispatcher.h"
#include "Data/Events/CollisionEvent.h"
#include "AI/IARandomMovementComponent.h"
#include "ECS/Basics/Graphic/DialogComponent.h"

void LoadRawStatistics(RawStatistics<int>& stats, ska::IniReader& data, const std::string& block);

void LoadRawStatistics(RawStatistics<int>& stats, ska::IniReader& data, const std::string& block) {
	stats.hp = data.get<int>(block + " hp");
	stats.defense = data.get<int>(block + " defense");
	stats.speDefense = data.get<int>(block + " spe_defense");
	stats.speAttack = data.get<int>(block + " spe_attack");
	stats.speed = data.get<int>(block + " speed") / 2;
	stats.attack = data.get<int>(block + " attack");
}

SceneFight::SceneFight(CustomEntityManager& em, PokemonGameEventDispatcher& ged, ska::Window& w, ska::InputContextManager& ril, ska::Scene& oldScene, WorldScene& ws, ska::Point<int> fightPos, FightComponent fc) :
AbstractSceneMap_(em, ged, w, ril, oldScene, ws, true),
m_iaICM(ska::InputContextManager::instantiateEmpty(ril)),
m_opponentScriptId(fc.opponentScriptId),
m_level(fc.level),
m_opponent("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Monsters" FILE_SEPARATOR + ska::StringUtils::intToStr(fc.opponentScriptId) + ".ini"),
m_pokemon("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Monsters" FILE_SEPARATOR + ska::StringUtils::intToStr(fc.pokemonScriptId) + ".ini"),
m_pokemonId(fc.fighterPokemon),
m_trainerId(fc.trainer),
m_opponentId(fc.fighterOpponent),
m_sceneLoaded(false),
m_loadState(0),
m_worldEntityCollisionResponse(ws.getWorld(), ged, m_entityManager),
m_skillEntityCollisionResponse(*m_collisionSystem, ged, m_entityManager) {

	m_cameraSystem = addLogic<ska::CameraFixedSystem>(m_window.getWidth(), m_window.getHeight(), fightPos);
	addLogic<PokeballSystem>();
	addLogic<BattleSystem>(m_inputCManager, m_iaICM, fc.fighterPokemon, fc.fighterOpponent, m_pokemon, m_opponent);
	addLogic<SkillRefreshSystem>();
	addLogic<StatisticsSystem>(w, ril, ws, ged);
	addLogic<ska::IARandomMovementSystem>();

	addSubScene<SceneGUIBattle>();

	//TODO add IA input context ???
	//m_iaICM.addContext(ska::InputContextPtr());
}

ska::CameraSystem& SceneFight::getCamera() {
	return *m_cameraSystem;
}

void SceneFight::createSkill(SkillDescriptor& sd, const std::string& skillPath) const{
	ska::IniReader skillData(skillPath);

	if (!skillData.isLoaded()) {
		return;
	}

	sd.style1 = skillData.get<std::string>("Description style_1");
	sd.style2 = skillData.get<std::string>("Description style_2");
	sd.description = skillData.get<std::string>("Description description");
	sd.name = skillData.get<std::string>("Description name");
	sd.type = skillData.get<std::string>("Description type");
	sd.context = skillData.get<std::string>("Description context");

	const std::string& particleNumber = "Particle number";
	const std::string& particleSpeed = "Particle speed";
	const std::string& particleKnockback = "Particle knockback";
	const std::string& particleNoise = "Particle noise";
	const std::string& particleAmplitude = "Particle amplitude";

	sd.particleNumber = skillData.exists(particleNumber) ? skillData.get<int>(particleNumber) : 0;
	sd.speed = skillData.exists(particleSpeed) ? skillData.get<float>(particleSpeed) : 0;
	sd.knockback = static_cast<int>(skillData.exists(particleKnockback) ? skillData.get<float>(particleKnockback) : 0);
	sd.noise = static_cast<int>(skillData.exists(particleNoise) ? skillData.get<float>(particleNoise) : 0);
	sd.amplitude = skillData.exists(particleAmplitude) ? skillData.get<float>(particleAmplitude) : 0;;


	sd.id = skillData.get<int>("Description id");

	sd.cooldown = skillData.get<unsigned int>("Stats cooldown");
	sd.range = skillData.get<int>("Stats blocks_range") * static_cast<int>(m_worldScene.getWorld().getBlockSize());

	if (sd.style1 == "Buff" || sd.style2 == "Buff") {
		LoadRawStatistics(sd.buffAlly, skillData, "BuffsAlly");
		LoadRawStatistics(sd.buffEnemy, skillData, "BuffsEnemy");
		sd.alterAlly = skillData.get<int>("StatusAlter ally");
		sd.alterEnemy = skillData.get<int>("StatusAlter enemy");
	}
}

void SceneFight::loadSkills(const ska::IniReader& reader, const ska::EntityId, SkillsHolderComponent& shc) const{
	for (unsigned int i = 0; reader.exists("Skills " + ska::StringUtils::intToStr(static_cast<int>(i))) && i < shc.skills.size(); i++) {
		if (reader.get<unsigned int>("Skills " + ska::StringUtils::intToStr(static_cast<int>(i)) + "_level") <= m_level) {
			createSkill(shc.skills[i], reader.get<std::string>("Skills " + ska::StringUtils::intToStr(static_cast<int>(i))));
		}
	}
}

void SceneFight::beforeLoad(ska::ScenePtr* lastScene) {
	AbstractSceneMap::beforeLoad(lastScene);
	if (m_sceneLoaded) {
		return;
	}

	SkillsHolderComponent shc;
	loadSkills(m_pokemon, m_pokemonId, shc);
	m_entityManager.addComponent<SkillsHolderComponent>(m_pokemonId, shc);

	SkillsHolderComponent shcOpponent;
	loadSkills(m_opponent, m_opponentId, shcOpponent);

	m_entityManager.addComponent<SkillsHolderComponent>(m_opponentId, shcOpponent);

	m_descriptor.load(m_opponent, "Description");


	auto delay = 3000;

	ska::RepeatableTask<ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>* dialogRawTask;
	auto dialogTask = ska::RunnablePtr(dialogRawTask = new ska::RepeatableTask<ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>([&, delay](ska::Task<bool, ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>& t) {
		if (m_loadState == 0) {
			m_loadState++;

			if (!m_entityManager.hasComponent<ska::InputComponent>(m_trainerId)) {
				return false;
			}

			auto& ic = m_entityManager.getComponent<ska::InputComponent>(m_trainerId);
			m_entityManager.removeComponent<ska::InputComponent>(m_trainerId);

			ska::GUIEvent ge(ska::GUIEventType::ADD_BALLOON);
			ge.text = "Un " + m_descriptor.getName() + " sauvage apparaît !";
			ge.delay = delay;
			ge.windowName = "trainerBalloon";
			m_eventDispatcher.ska::Observable<ska::GUIEvent>::notifyObservers(ge);

			ska::DialogComponent dc;
			dc.handle = ge.balloonHandle;
			dc.name = ge.windowName;

			m_entityManager.addComponent<ska::DialogComponent>(m_trainerId, dc);
			t.forward(ic);
			return true;
		}
		// Continue until dialog is still visible
		return m_entityManager.hasComponent<ska::DialogComponent>(m_trainerId);
	}));

	ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<ska::InputComponent, ska::EntityId>>* pokeballRawTask;
	auto pokeballTask = ska::RunnablePtr(pokeballRawTask = new ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<ska::InputComponent, ska::EntityId>>([&](ska::Task<bool, ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<ska::InputComponent, ska::EntityId>>& t, ska::InputComponent ic) {
		static ska::EntityId pokeball;
		if (m_loadState == 1) {
			m_loadState++;

			auto& pc = m_entityManager.getComponent<ska::PositionComponent>(m_trainerId);
			auto& hc = m_entityManager.getComponent<ska::HitboxComponent>(m_trainerId);
			auto& opponentPc = m_entityManager.getComponent<ska::PositionComponent>(m_opponentId);
			auto& opponentHc = m_entityManager.getComponent<ska::HitboxComponent>(m_opponentId);

			/* Création d'une entité : Pokéball (Position + Pokeball) */
			pokeball = m_entityManager.createEntity();
			PokeballComponent pokeballc;

			pokeballc.finalPos = { static_cast<int>(opponentPc.x + opponentHc.xOffset + opponentHc.width / 2), static_cast<int>(opponentPc.y + opponentHc.yOffset + opponentHc.height / 2) };
			m_entityManager.addComponent<PokeballComponent>(pokeball, pokeballc);
			ska::PositionComponent pokePc;
			pokePc = pc;
			pokePc.x += hc.xOffset + hc.width/2;
			pokePc.y += hc.yOffset + hc.height/2;
			m_entityManager.addComponent<ska::PositionComponent>(pokeball, pokePc);
			t.forward(ic, pokeball);
			return true;
		}
		return m_entityManager.hasComponent<PokeballComponent>(pokeball)/*wScreen.getPokeball().isVisible()*/;
	}, *dialogRawTask));

	ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent, ska::EntityId>, ska::TaskSender<>>* finalRawTask;
	auto finalTask = ska::RunnablePtr(finalRawTask = new ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent, ska::EntityId>, ska::TaskSender<>>(
		[&](ska::Task<bool, ska::TaskReceiver<ska::InputComponent, ska::EntityId>, ska::TaskSender<>>&, ska::InputComponent ic, ska::EntityId pokeball) {

		/* Ajout InputComponent au Pokémon,
		   Ajout d'un IAMovementComponent au dresseur (m_player),
		   Ajout d'un composant de combat au Pokémon
		   Ajout d'une HP Bar */

		m_entityManager.addComponent<BattleComponent>(m_pokemonId, BattleComponent());
		m_entityManager.addComponent<BattleComponent>(m_opponentId, BattleComponent());

		ska::IARandomMovementComponent iarmc;
		iarmc.emitter = m_pokemonId;
		iarmc.delay = 300;
		iarmc.type = ska::RandomMovementType::CIRCLE_AROUND;

		m_entityManager.addComponent<ska::IARandomMovementComponent>(m_trainerId, iarmc);
		m_entityManager.addComponent<ska::InputComponent>(m_pokemonId, ic);

		auto& hc = m_entityManager.getComponent<ska::HitboxComponent>(m_pokemonId);
		auto& pc = m_entityManager.getComponent<ska::PositionComponent>(pokeball);
		ska::Rectangle hitbox{ pc.x + hc.xOffset, pc.y + hc.yOffset, static_cast<int>(hc.width), static_cast<int>(hc.height) };

		const auto targetBlock = m_worldScene.getWorld().placeOnNearestPracticableBlock(hitbox, 1);
		pc.x = targetBlock.x - hc.xOffset;
		pc.y = targetBlock.y - hc.yOffset;
		m_entityManager.addComponent<ska::PositionComponent>(m_pokemonId, pc);

		BattleEvent be(BATTLE_START, *m_cameraSystem, m_pokemonId, m_opponentId, m_entityManager);
		m_eventDispatcher.ska::Observable<BattleEvent>::notifyObservers(be);

		m_sceneLoaded = true;
		m_loadState = 0;
		return false;
	}, *pokeballRawTask));


	queueTask(dialogTask);
	queueTask(pokeballTask);
	queueTask(finalTask);
}

bool SceneFight::beforeUnload() {
	AbstractSceneMap::beforeUnload();
	//m_worldScene.unload();

	/* Triggers end fight cinematic to the next scene */
	ska::RepeatableTask<ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>* dialogRawTask;
	const auto delay = 3000U;

	auto dialogTask = ska::RunnablePtr(dialogRawTask = new ska::RepeatableTask<ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>([&, delay](ska::Task<bool, ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>& t) {
		static ska::EntityId pokeball = -1;
		if (m_loadState == 0) {
			m_loadState++;

			if (!m_entityManager.hasComponent<ska::InputComponent>(m_pokemonId)) {
				return false;
			}

			auto& ic = m_entityManager.getComponent<ska::InputComponent>(m_pokemonId);
			m_entityManager.removeComponent<ska::InputComponent>(m_pokemonId);

			BattleEvent be(BATTLE_END, *m_cameraSystem, m_pokemonId, m_opponentId, m_entityManager);
			m_eventDispatcher.ska::Observable<BattleEvent>::notifyObservers(be);

			m_entityManager.removeComponent<BattleComponent>(m_pokemonId);
			m_entityManager.removeComponent<BattleComponent>(m_opponentId);

			if (m_entityManager.hasComponent<ska::DirectionalAnimationComponent>(m_trainerId)) {
				auto& dac = m_entityManager.getComponent<ska::DirectionalAnimationComponent>(m_trainerId);
				dac.type = ska::DirectionalAnimationType::MOVEMENT;
				dac.looked = 0;
			}

			ska::GUIEvent ge(ska::GUIEventType::ADD_BALLOON);
			ge.text = "Le " + m_descriptor.getName() + " a été battu.";
			ge.delay = delay;
			ge.windowName = "trainerBalloon";
			m_eventDispatcher.ska::Observable<ska::GUIEvent>::notifyObservers(ge);

			ska::DialogComponent dc;
			dc.handle = ge.balloonHandle;
			dc.name = ge.windowName;
			m_entityManager.addComponent<ska::DialogComponent>(m_trainerId, dc);

			/* Création d'une entité : Pokéball (Position + Pokeball) */
			pokeball = m_entityManager.createEntity();
			auto& pokemonPc = m_entityManager.getComponent<ska::PositionComponent>(m_pokemonId);
			auto& pokemonHp = m_entityManager.getComponent<ska::HitboxComponent>(m_pokemonId);
			auto& pc = m_entityManager.getComponent<ska::PositionComponent>(m_trainerId);
			auto& hc = m_entityManager.getComponent<ska::HitboxComponent>(m_trainerId);
			PokeballComponent pokeballc;

			pokeballc.finalPos = { static_cast<int>(pokemonPc.x + pokemonHp.xOffset + pokemonHp.width / 2), static_cast<int>(pokemonPc.y + pokemonHp.yOffset + pokemonHp.height / 2) };
			m_entityManager.addComponent<PokeballComponent>(pokeball, pokeballc);
			ska::PositionComponent pokePc;
			pokePc = pc;
			pokePc.x += hc.xOffset + hc.width / 2;
			pokePc.y += hc.yOffset + hc.height / 2;
			m_entityManager.addComponent<ska::PositionComponent>(pokeball, pokePc);

			t.forward(ic);
			return true;
		}
		// Continue until dialog and pokeball is still visible
		return m_entityManager.hasComponent<ska::DialogComponent>(m_trainerId) ||
			m_entityManager.hasComponent<PokeballComponent>(pokeball);
	}));

	ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<>>* finalRawTask;
	auto finalTask = ska::RunnablePtr(finalRawTask = new ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<>>(
		[&](ska::Task<bool, ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<>>&, ska::InputComponent ic) {

		m_loadState = 0;
		m_entityManager.removeComponent<ska::IARandomMovementComponent>(m_trainerId);
		m_entityManager.addComponent<ska::InputComponent>(m_trainerId, ic);
		m_entityManager.removeEntity(m_pokemonId);
		return false;
	}, *dialogRawTask));

	if (m_sceneLoaded) {
		m_sceneLoaded = false;
		queueTask(dialogTask);
		queueTask(finalTask);
	}
	return false;
}

SceneFight::~SceneFight() {
}
