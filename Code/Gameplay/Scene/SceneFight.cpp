#include "../World/WorldScene.h"
#include "SceneFight.h"
#include "../../ska/Task/RepeatableTask.h"
#include "../CustomEntityManager.h"
#include "../Fight/FightComponent.h"
#include "../Data/PokemonDescriptor.h"
#include "../../ska/Task/TaskQueue.h"
#include "../../Graphic/GUI/Bar.h"

#include "../PokemonGameEventDispatcher.h"
#include "../../ska/Physic/CollisionEvent.h"
#include "../../ska/AI/IARandomMovementComponent.h"

void LoadRawStatistics(RawStatistics<int>& stats, ska::IniReader& data, const std::string& block) {
	stats.hp = data.get<int>(block + " hp");
	stats.defense = data.get<int>(block + " defense");
	stats.speDefense = data.get<int>(block + " spe_defense");
	stats.speAttack = data.get<int>(block + " spe_attack");
	stats.speed = data.get<int>(block + " speed") / 2;
	stats.attack = data.get<int>(block + " attack");
}

SceneFight::SceneFight(ska::Window& w, ska::SceneHolder& sh, WorldScene& ws, ska::InputContextManager& ril, ska::Point<int> fightPos, FightComponent fc, PokemonGameEventDispatcher& ged) :
AbstractSceneMap_(w, ws, ged, sh, ril, true),
m_ged(ged),
m_iaICM(ska::InputContextManager::instantiateEmpty(ril)),
m_cameraSystem(ws.getEntityManager(), ws.getScreenW(), ws.getScreenH(), fightPos),
m_pokeballSystem(ws.getEntityManager()),
m_statsSystem(w, ws.getEntityManager(), sh, ril, ws, ged),
m_opponentScriptId(fc.opponentScriptId),
m_level(fc.level),
m_opponent("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Monsters" FILE_SEPARATOR + ska::StringUtils::intToStr(fc.opponentScriptId) + ".ini"),
m_pokemon("." FILE_SEPARATOR "Data" FILE_SEPARATOR "Monsters" FILE_SEPARATOR + ska::StringUtils::intToStr(fc.pokemonScriptId) + ".ini"),
m_pokemonId(fc.fighterPokemon),
m_trainerId(fc.trainer),
m_opponentId(fc.fighterOpponent),
m_sceneLoaded(false),
m_loadState(0),
m_battleSystem(ws.getEntityManager(), m_inputCManager, m_iaICM, fc.fighterPokemon, fc.fighterOpponent, m_pokemon, m_opponent),
m_skillRefreshSystem(ws.getEntityManager()),
m_worldEntityCollisionResponse(ws.getWorld(), ged, ws.getEntityManager()),
m_skillEntityCollisionResponse(m_collisionSystem, ged, ws.getEntityManager()),
m_randomMovementSystem(ws.getEntityManager()),
m_guiBattle(w, ril, ged),
m_taskQueue(sh) {
	m_logics.push_back(&m_cameraSystem);
	m_logics.push_back(&m_pokeballSystem);
	m_logics.push_back(&m_battleSystem);
	m_logics.push_back(&m_skillRefreshSystem);
	m_logics.push_back(&m_statsSystem);
	m_logics.push_back(&m_randomMovementSystem);

	ged.ska::Observable<ska::CollisionEvent>::removeObserver(m_entityCollisionResponse);
	ged.ska::Observable<ska::CollisionEvent>::removeObserver(m_worldCollisionResponse);

	//TODO add IA input context ???
	//m_iaICM.addContext(ska::InputContextPtr());
}

ska::CameraSystem& SceneFight::getCamera() {
	return m_cameraSystem;
}

void SceneFight::graphicUpdate(ska::DrawableContainer& drawables) {
	AbstractSceneMap::graphicUpdate(drawables);

	//Affiche l'UI des combats et les attaques (disposé après le dessin de la Pokéball)
	m_guiBattle.graphicUpdate(drawables);
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

void SceneFight::load(ska::ScenePtr* lastScene) {
	if (m_sceneLoaded) {
		return;
	}

	AbstractSceneMap::load(lastScene);

	m_worldScene.linkCamera(&m_cameraSystem);
	m_worldScene.load(lastScene);

	SkillsHolderComponent shc;
	loadSkills(m_pokemon, m_pokemonId, shc);
	m_worldScene.getEntityManager().addComponent<SkillsHolderComponent>(m_pokemonId, shc);

	SkillsHolderComponent shcOpponent;
	loadSkills(m_opponent, m_opponentId, shcOpponent);

	m_worldScene.getEntityManager().addComponent<SkillsHolderComponent>(m_opponentId, shcOpponent);

	m_descriptor.load(m_opponent, "Description");


	auto delay = 3000;

	ska::RepeatableTask<ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>* dialogRawTask;
	auto dialogTask = ska::RunnablePtr(dialogRawTask = new ska::RepeatableTask<ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>([&, delay](ska::Task<bool, ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>& t) {
		//static int dialogId = 0;
		if (m_loadState == 0) {
			m_loadState++;

			if (!m_worldScene.getEntityManager().hasComponent<ska::InputComponent>(m_trainerId)) {
				return false;
			}

			auto& ic = m_worldScene.getEntityManager().getComponent<ska::InputComponent>(m_trainerId);
			m_worldScene.getEntityManager().removeComponent<ska::InputComponent>(m_trainerId);

			//auto& pc = m_worldScene.getEntityManager().getComponent<ska::PositionComponent>(m_trainerId);
			//DialogComponent dc(DialogMenu("Un " + m_descriptor.getName() + " sauvage apparaît !", { 0, TAILLEBLOCFENETRE * 2, TAILLEBLOCFENETRE * 10, TAILLEBLOCFENETRE * 2 }, delay, false));
			//dc.dialog.hide(false);

			//m_worldScene.getEntityManager().addComponent<DialogComponent>(m_trainerId, dc);
			t.forward(ic);
			return true;
		}
		// Continue until dialog is still visible
		//return m_worldScene.getEntityManager().hasComponent<DialogComponent>(m_trainerId);
		return false;
	}));

	ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<ska::InputComponent, ska::PositionComponent>>* pokeballRawTask;
	auto pokeballTask = ska::RunnablePtr(pokeballRawTask = new ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<ska::InputComponent, ska::PositionComponent>>([&](ska::Task<bool, ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<ska::InputComponent, ska::PositionComponent>>& t, ska::InputComponent ic) {
		static ska::EntityId pokeball;
		if (m_loadState == 1) {
			m_loadState++;

			auto& pc = m_worldScene.getEntityManager().getComponent<ska::PositionComponent>(m_trainerId);
			auto& hc = m_worldScene.getEntityManager().getComponent<ska::HitboxComponent>(m_trainerId);
			auto& opponentPc = m_worldScene.getEntityManager().getComponent<ska::PositionComponent>(m_opponentId);
			auto& opponentHc = m_worldScene.getEntityManager().getComponent<ska::HitboxComponent>(m_opponentId);

			/* Création d'une entité : Pokéball (Position + Pokeball) */
			pokeball = m_worldScene.getEntityManager().createEntity();
			PokeballComponent pokeballc;

			pokeballc.finalPos = { static_cast<int>(opponentPc.x + opponentHc.xOffset + opponentHc.width / 2), static_cast<int>(opponentPc.y + opponentHc.yOffset + opponentHc.height / 2) };
			m_worldScene.getEntityManager().addComponent<PokeballComponent>(pokeball, pokeballc);
			ska::PositionComponent pokePc;
			pokePc = pc;
			pokePc.x += hc.xOffset + hc.width/2;
			pokePc.y += hc.yOffset + hc.height/2;
			m_worldScene.getEntityManager().addComponent<ska::PositionComponent>(pokeball, pokePc);
			t.forward(ic, pokePc);
			return true;
		}
		return m_worldScene.getEntityManager().hasComponent<PokeballComponent>(pokeball)/*wScreen.getPokeball().isVisible()*/;
	}, *dialogRawTask));

	ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent, ska::PositionComponent>, ska::TaskSender<>>* finalRawTask;
	auto finalTask = ska::RunnablePtr(finalRawTask = new ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent, ska::PositionComponent>, ska::TaskSender<>>([&](ska::Task<bool, ska::TaskReceiver<ska::InputComponent, ska::PositionComponent>, ska::TaskSender<>>&, ska::InputComponent ic, ska::PositionComponent pc) {

		/* Ajout InputComponent au Pokémon,
		   Ajout d'un IAMovementComponent au dresseur (m_player),
		   Ajout d'un composant de combat au Pokémon
		   Ajout d'une HP Bar */

		m_worldScene.getEntityManager().addComponent<BattleComponent>(m_pokemonId, BattleComponent());
		m_worldScene.getEntityManager().addComponent<BattleComponent>(m_opponentId, BattleComponent());

		ska::IARandomMovementComponent iarmc;
		iarmc.emitter = m_pokemonId;
		iarmc.delay = 300;
		iarmc.type = ska::RandomMovementType::CIRCLE_AROUND;

		m_worldScene.getEntityManager().addComponent<ska::IARandomMovementComponent>(m_trainerId, iarmc);
		m_worldScene.getEntityManager().addComponent<ska::InputComponent>(m_pokemonId, ic);

		auto& hc = m_worldScene.getEntityManager().getComponent<ska::HitboxComponent>(m_pokemonId);
		ska::Rectangle hitbox{ pc.x + hc.xOffset, pc.y + hc.yOffset, static_cast<int>(hc.width), static_cast<int>(hc.height) };

		const auto targetBlock = m_worldScene.getWorld().placeOnNearestPracticableBlock(hitbox, 1);
		pc.x = targetBlock.x - hc.xOffset;
		pc.y = targetBlock.y - hc.yOffset;
		m_worldScene.getEntityManager().addComponent<ska::PositionComponent>(m_pokemonId, pc);

		BattleEvent be(BATTLE_START, m_cameraSystem, m_pokemonId, m_opponentId, m_worldScene.getEntityManager());
		m_ged.ska::Observable<BattleEvent>::notifyObservers(be);

		m_sceneLoaded = true;
		m_loadState = 0;
		return false;
	}, *pokeballRawTask));


	m_taskQueue.queueTask(dialogTask);
	m_taskQueue.queueTask(pokeballTask);
	m_taskQueue.queueTask(finalTask);

}

bool SceneFight::unload() {
	m_worldScene.unload();

	/* Triggers end fight cinematic to the next scene */
	ska::RepeatableTask<ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>* dialogRawTask;
	const unsigned int delay = 3000;

	auto dialogTask = ska::RunnablePtr(dialogRawTask = new ska::RepeatableTask<ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>([&, delay](ska::Task<bool, ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>& t) {
		//static auto dialogId = 0;
		if (m_loadState == 0) {
			m_loadState++;

			/* Resets GUI */
			m_guiBattle.clear();

			if (!m_worldScene.getEntityManager().hasComponent<ska::InputComponent>(m_pokemonId)) {
				return false;
			}

			auto& ic = m_worldScene.getEntityManager().getComponent<ska::InputComponent>(m_pokemonId);
			m_worldScene.getEntityManager().removeComponent<ska::InputComponent>(m_pokemonId);
			//ska::PositionComponent& pc = m_worldScene.getEntityManager().getComponent<ska::PositionComponent>(m_trainerId);
			//DialogComponent dc(DialogMenu("Le " + m_descriptor.getName() + " a été battu.", { 0, TAILLEBLOCFENETRE * 2, TAILLEBLOCFENETRE * 10, TAILLEBLOCFENETRE * 2 }, delay, false));
			//dc.dialog.hide(false);

			//m_worldScene.getEntityManager().addComponent<DialogComponent>(m_trainerId, dc);
			t.forward(ic);
			return true;
		}
		// Continue until dialog is still visible
		//return m_worldScene.getEntityManager().hasComponent<DialogComponent>(m_trainerId);
		return false;
	}));

	ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<>>* finalRawTask;
	auto finalTask = ska::RunnablePtr(finalRawTask = new ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<>>(
		[&](ska::Task<bool, ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<>>&, ska::InputComponent ic) {

		m_loadState = 0;
		m_worldScene.unload();
		m_worldScene.getEntityManager().removeComponent<ska::IARandomMovementComponent>(m_trainerId);
		m_worldScene.getEntityManager().addComponent<ska::InputComponent>(m_trainerId, ic);
		m_worldScene.getEntityManager().removeEntity(m_pokemonId);
		return false;
	}, *dialogRawTask));

	if (m_sceneLoaded) {
		m_sceneLoaded = false;
		m_taskQueue.queueTask(dialogTask);
		m_taskQueue.queueTask(finalTask);
	}
	return m_taskQueue.hasRunningTask();
}

void SceneFight::eventUpdate(bool movingDisallowed) {
	AbstractSceneMap::eventUpdate(movingDisallowed);
	m_guiBattle.eventUpdate(movingDisallowed);
}

SceneFight::~SceneFight() {
}
