#include "../World/WorldScene.h"
#include "SceneFight.h"
#include "../../ska/Task/RepeatableTask.h"
#include "../WGameCore.h"
#include "../CustomEntityManager.h"
#include "../Fight/FightComponent.h"
#include "../Data/PokemonDescriptor.h"
#include "../../Graphic/DialogComponent.h"
#include "../../Utils/IDs.h"
#include "../../Graphic/GUI/DialogMenu.h"
#include "../Fight/SkillsHolderComponent.h"
#include "../Data/Statistics.h"

typedef std::unique_ptr<DialogMenu> DialogMenuPtr;

SceneFight::SceneFight(ska::SceneHolder& sh, WorldScene& ws, ska::InputContextManager& ril, ska::Point<int> fightPos, FightComponent fc) :
AbstractSceneMap(sh, ril),
m_iaICM(ska::InputContextManager::instantiateEmpty(ril)),
m_worldScene(ws),
m_statsSystem(ws.getEntityManager(), sh, ril, ws),
m_cameraSystem(ws.getEntityManager(), ws.getScreenW(), ws.getScreenH(), fightPos),
m_pokeballSystem(ws.getEntityManager()),
m_opponentScriptId(fc.opponentScriptId),
m_level(fc.level),
m_pokemonId(fc.fighterPokemon),
m_trainerId(fc.trainer),
m_opponentId(fc.fighterOpponent),
m_pokemon("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Monsters"FILE_SEPARATOR + ska::StringUtils::intToStr(fc.pokemonScriptId) + ".ini"),
m_opponent("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Monsters"FILE_SEPARATOR + ska::StringUtils::intToStr(fc.opponentScriptId) + ".ini"),
m_battleSystem(ws.getEntityManager(), m_inputCManager, m_iaICM, fc.fighterPokemon, fc.fighterOpponent, m_pokemon, m_opponent),
m_skillRefreshSystem(ws.getEntityManager()),
m_collisionSystem(ws.getWorld(), ws.getEntityManager()),
m_sceneLoaded(false),
m_worldCollisionResponse(ws.getWorld(), m_collisionSystem, ws.getEntityManager()),
m_entityCollisionResponse(m_collisionSystem, ws.getEntityManager()),
m_skillEntityCollisionResponse(m_collisionSystem, ws.getEntityManager()) {
	m_logics.push_back(&m_cameraSystem);
	m_logics.push_back(&m_pokeballSystem);
	m_logics.push_back(&m_battleSystem);
	m_logics.push_back(&m_skillRefreshSystem);
	m_logics.push_back(&m_collisionSystem);
	m_logics.push_back(&m_statsSystem);

	//TODO add IA input context
	//m_iaICM.addContext(ska::InputContextPtr());
}

void SceneFight::graphicUpdate(ska::DrawableContainer& drawables) {
	 //WGameCore& core = WGameCore::getInstance();
	/*ParticleManager& particleManager = core.getParticleManager();
	MouseCursor& mouseCursor = core.getMouseCursor();
	Settings& settings = core.getSettings();*/

	m_worldScene.graphicUpdate(drawables);
	AbstractSceneMap::graphicUpdate(drawables);

	//Affiche l'UI des combats et les attaques (disposé après le dessin de la Pokéball)
	/*fight.setPriority(pokeball.getPriority() + 1);*/
	/*drawables.add(fight);*/
}


void SceneFight::createSkill(SkillDescriptor& sd, const std::string& skillPath) {
	ska::IniReader skillData(skillPath);

	sd.style1 = skillData.getString("Description style_1");
	sd.style2 = skillData.getString("Description style_2");
	sd.description = skillData.getString("Description description");
	sd.name = skillData.getString("Description name");
	sd.type = skillData.getString("Description type");
	sd.context = skillData.getInt("Description context");
	sd.particleNumber = skillData.getInt("Particle number");
	sd.speed = skillData.getFloat("Particle speed");
	sd.knockback = skillData.getInt("Particle knockback");
	sd.noise = skillData.getInt("Particle noise");
	sd.amplitude = skillData.getFloat("Particle amplitude");


	sd.id = skillData.getInt("Description id");


	/* TODO put the sprite in the GUI */
	//m_icone.load("."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR + ska::StringUtils::intToStr(m_id) + ".png", DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);

	sd.cooldown = skillData.getInt("Stats cooldown");
	sd.range = skillData.getInt("Stats blocks_range") * m_worldScene.getWorld().getBlockSize();

	if (sd.style1 == "Buff" || sd.style2 == "Buff") {
		sd.buffAlly = Statistics(&skillData, "BuffsAlly").getRawStats();
		sd.buffEnemy = Statistics(&skillData, "BuffsEnemy").getRawStats();
		sd.alterAlly = skillData.getInt("StatusAlter ally");
		sd.alterEnemy = skillData.getInt("StatusAlter enemy");
	}
}

void SceneFight::loadSkills(const ska::IniReader& reader, const ska::EntityId entity, SkillsHolderComponent& shc) {
	for (unsigned int i = 0; reader.get("Skills " + ska::StringUtils::intToStr(i)) && i < shc.skills.size(); i++) {
		if (reader.getInt("Skills " + ska::StringUtils::intToStr(i) + "_level") <= m_level) {
			createSkill(shc.skills[i], reader.getString("Skills " + ska::StringUtils::intToStr(i)));
		}
	}
}

void SceneFight::load(ska::ScenePtr* lastScene) {
	if (m_sceneLoaded) {
		return;
	}

	m_worldScene.linkCamera(&m_cameraSystem);
	m_worldScene.load(lastScene);

	SkillsHolderComponent shc;
	loadSkills(m_pokemon, m_pokemonId, shc);
	m_worldScene.getEntityManager().addComponent<SkillsHolderComponent>(m_pokemonId, shc);

	SkillsHolderComponent shcOpponent;
	loadSkills(m_opponent, m_opponentId, shcOpponent);
	
	m_worldScene.getEntityManager().addComponent<SkillsHolderComponent>(m_opponentId, shcOpponent);
	
	m_descriptor.load(m_opponent, "Description");
	
	
	int delay = 3000;

	ska::RepeatableTask<ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>* dialogRawTask;
	ska::RunnablePtr dialogTask = ska::RunnablePtr(dialogRawTask = new ska::RepeatableTask<ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>([&, delay](ska::Task<bool, ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>& t) {
		static bool started = false;
		static int dialogId = 0;
		if (!started) {
			started = true;

			if (!m_worldScene.getEntityManager().hasComponent<ska::InputComponent>(m_trainerId)) {
				return false;
			}

			ska::InputComponent& ic = m_worldScene.getEntityManager().getComponent<ska::InputComponent>(m_trainerId);
			m_worldScene.getEntityManager().removeComponent<ska::InputComponent>(m_trainerId);

			ska::PositionComponent& pc = m_worldScene.getEntityManager().getComponent<ska::PositionComponent>(m_trainerId);
			DialogComponent dc(DialogMenu("Un " + m_descriptor.getName() + " sauvage apparaît !", { 0, TAILLEBLOCFENETRE * 2, TAILLEBLOCFENETRE * 10, TAILLEBLOCFENETRE * 2 }, delay, false));
			dc.dialog.hide(false);

			m_worldScene.getEntityManager().addComponent<DialogComponent>(m_trainerId, dc);
			t.forward(ic);
			return true;
		}
		// Continue until dialog is still visible
		return m_worldScene.getEntityManager().hasComponent<DialogComponent>(m_trainerId);
	}));

	ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<ska::InputComponent, ska::PositionComponent>>* pokeballRawTask;
	ska::RunnablePtr pokeballTask = ska::RunnablePtr(pokeballRawTask = new ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<ska::InputComponent, ska::PositionComponent>>([&](ska::Task<bool, ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<ska::InputComponent, ska::PositionComponent>>& t, ska::InputComponent ic) {
		static bool started = false;
		static ska::EntityId pokeball;

		if (!started) {
			started = true;

			ska::PositionComponent& pc = m_worldScene.getEntityManager().getComponent<ska::PositionComponent>(m_trainerId);
			ska::HitboxComponent& hc = m_worldScene.getEntityManager().getComponent<ska::HitboxComponent>(m_trainerId);
			ska::PositionComponent& opponentPc = m_worldScene.getEntityManager().getComponent<ska::PositionComponent>(m_opponentId);
			ska::HitboxComponent& opponentHc = m_worldScene.getEntityManager().getComponent<ska::HitboxComponent>(m_opponentId);

			/* Création d'une entité : Pokéball (Position + Pokeball) */
			pokeball = m_worldScene.getEntityManager().createEntity();
			PokeballComponent pokeballc;
			
			pokeballc.finalPos = { (int)(opponentPc.x + opponentHc.xOffset + opponentHc.width / 2), (int)(opponentPc.y + opponentHc.yOffset + opponentHc.height / 2) };
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
	ska::RunnablePtr finalTask = ska::RunnablePtr(finalRawTask = new ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent, ska::PositionComponent>, ska::TaskSender<>>([&](ska::Task<bool, ska::TaskReceiver<ska::InputComponent, ska::PositionComponent>, ska::TaskSender<>>& t, ska::InputComponent ic, ska::PositionComponent pc) {
		
		/* Ajout InputComponent au Pokémon,
		   Ajout d'un IAMovementComponent au dresseur (m_player),
		   Ajout d'un composant de combat au Pokémon */
		m_worldScene.getEntityManager().addComponent<ska::InputComponent>(m_pokemonId, ic);

		/* TODO random position autour de l'ennemi */
		m_worldScene.getEntityManager().addComponent<ska::PositionComponent>(m_pokemonId, pc);
		m_worldScene.getEntityManager().addComponent<BattleComponent>(m_pokemonId, BattleComponent());
		m_worldScene.getEntityManager().addComponent<BattleComponent>(m_opponentId, BattleComponent());
		
		m_sceneLoaded = true;
		return false;
	}, *pokeballRawTask));

	WGameCore& wScreen = WGameCore::getInstance();
	wScreen.addTaskToQueue(dialogTask);
	wScreen.addTaskToQueue(pokeballTask);
	wScreen.addTaskToQueue(finalTask);
	
}

bool SceneFight::unload() {
	
	
	/* Triggers end fight cinematic to the next scene */
	ska::RepeatableTask<ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>* dialogRawTask;
	const unsigned int delay = 3000;

	ska::RunnablePtr dialogTask = ska::RunnablePtr(dialogRawTask = new ska::RepeatableTask<ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>([&, delay](ska::Task<bool, ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>& t) {
		static bool started = false;
		static int dialogId = 0;
		if (!started) {
			started = true;

			if (!m_worldScene.getEntityManager().hasComponent<ska::InputComponent>(m_pokemonId)) {
				return false;
			}

			ska::InputComponent& ic = m_worldScene.getEntityManager().getComponent<ska::InputComponent>(m_pokemonId);
			m_worldScene.getEntityManager().removeComponent<ska::InputComponent>(m_pokemonId);

			ska::PositionComponent& pc = m_worldScene.getEntityManager().getComponent<ska::PositionComponent>(m_trainerId);
			DialogComponent dc(DialogMenu("Le " + m_descriptor.getName() + " a été battu.", { 0, TAILLEBLOCFENETRE * 2, TAILLEBLOCFENETRE * 10, TAILLEBLOCFENETRE * 2 }, delay, false));
			dc.dialog.hide(false);

			m_worldScene.getEntityManager().addComponent<DialogComponent>(m_trainerId, dc);
			t.forward(ic);
			return true;
		}
		// Continue until dialog is still visible
		return m_worldScene.getEntityManager().hasComponent<DialogComponent>(m_trainerId);
	}));

	ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<>>* finalRawTask;
	ska::RunnablePtr finalTask = ska::RunnablePtr(finalRawTask = new ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<>>(
		[&](ska::Task<bool, ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<>>& t, ska::InputComponent ic) {

		m_worldScene.unload();
		m_worldScene.getEntityManager().addComponent<ska::InputComponent>(m_trainerId, ic);
		//m_worldScene.getEntityManager().removeEntity(m_pokemonId);
		return false;
	}, *dialogRawTask));

	WGameCore& wScreen = WGameCore::getInstance();
	if (m_sceneLoaded) {
		m_sceneLoaded = false;
		wScreen.addTaskToQueue(dialogTask);
		wScreen.addTaskToQueue(finalTask);
	}
	return wScreen.hasRunningTask();
}

void SceneFight::eventUpdate(bool movingDisallowed) {
	AbstractSceneMap::eventUpdate(movingDisallowed);
	m_worldScene.eventUpdate(movingDisallowed);
}

SceneFight::~SceneFight()
{
}
