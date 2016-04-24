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
m_worldScene(ws),
m_cameraSystem(ws.getEntityManager(), ws.getScreenW(), ws.getScreenH(), fightPos),
m_pokeballSystem(ws.getEntityManager()),
m_scriptId(fc.scriptId),
m_level(fc.level),
m_playerId(fc.fighterPlayer),
m_opponentId(fc.fighterOpponent),
m_player("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Monsters"FILE_SEPARATOR + ska::StringUtils::intToStr(m_playerId) + ".ini"),
m_opponent("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Monsters"FILE_SEPARATOR + ska::StringUtils::intToStr(m_scriptId) + ".ini") {
	m_logics.push_back(&m_cameraSystem);
	m_logics.push_back(&m_pokeballSystem);
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

	sd.id = skillData.getInt("Description id");


	/* TODO put the sprite in the GUI */
	//m_icone.load("."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR + ska::StringUtils::intToStr(m_id) + ".png", DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);

	sd.cooldown = skillData.getInt("Stats cooldown");
	sd.range = skillData.getInt("Stats blocks_range");

	if (sd.style1 == "Buff" || sd.style2 == "Buff") {
		sd.buffAlly = Statistics(&skillData, "BuffsAlly").getRawStats();
		sd.buffEnemy = Statistics(&skillData, "BuffsEnemy").getRawStats();
		sd.alterAlly = skillData.getInt("StatusAlter ally");
		sd.alterEnemy = skillData.getInt("StatusAlter enemy");
	}
}

void SceneFight::loadSkills(const ska::IniReader& reader, SkillsHolderComponent& shc) {
	for (unsigned int i = 0; reader.get("Skills " + ska::StringUtils::intToStr(i)) && i < shc.skills.size(); i++) {
		if (reader.getInt("Skills " + ska::StringUtils::intToStr(i) + "_level") <= m_level) {
			createSkill(shc.skills[i], reader.getString("Skills " + ska::StringUtils::intToStr(i)));
		}

	}
}

void SceneFight::load() {
	m_worldScene.linkCamera(&m_cameraSystem);
	m_worldScene.load();

	SkillsHolderComponent shc;
	loadSkills(m_player, shc);
	m_worldScene.getEntityManager().addComponent<SkillsHolderComponent>(m_playerId, shc);

	SkillsHolderComponent shcOpponent;
	loadSkills(m_opponent, shcOpponent);
	m_worldScene.getEntityManager().addComponent<SkillsHolderComponent>(m_opponentId, shcOpponent);
	
	m_descriptor.load(m_opponent, "Description");
	
	
	int delay = 3000;

	ska::RepeatableTask<ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>* dialogRawTask;
	ska::RunnablePtr dialogTask = ska::RunnablePtr(dialogRawTask = new ska::RepeatableTask<ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>([&, delay](ska::Task<bool, ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>& t) {
		static bool started = false;
		static int dialogId = 0;
		if (!started) {
			started = true;

			if (!m_worldScene.getEntityManager().hasComponent<ska::InputComponent>(m_playerId)) {
				return false;
			}

			ska::InputComponent& ic = m_worldScene.getEntityManager().getComponent<ska::InputComponent>(m_playerId);
			m_worldScene.getEntityManager().removeComponent<ska::InputComponent>(m_playerId);

			ska::PositionComponent& pc = m_worldScene.getEntityManager().getComponent<ska::PositionComponent>(m_playerId);
			DialogComponent dc(DialogMenu("Un " + m_descriptor.getName() + " sauvage apparaît !", { 0, 0, TAILLEBLOCFENETRE * 10, TAILLEBLOCFENETRE * 2 }, delay, false));
			dc.dialog.hide(false);

			m_worldScene.getEntityManager().addComponent<DialogComponent>(m_playerId, dc);
			t.forward(ic);
			return true;
		}
		// Continue until dialog is still visible
		return m_worldScene.getEntityManager().hasComponent<DialogComponent>(m_playerId);
	}));

	ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<ska::InputComponent>>* pokeballRawTask;
	ska::RunnablePtr pokeballTask = ska::RunnablePtr(pokeballRawTask = new ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<ska::InputComponent>>([&](ska::Task<bool, ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<ska::InputComponent>>& t, ska::InputComponent ic) {
		static bool started = false;
		static ska::EntityId pokeball;

		if (!started) {
			started = true;
			t.forward(ic);

			ska::PositionComponent& pc = m_worldScene.getEntityManager().getComponent<ska::PositionComponent>(m_playerId);
			ska::HitboxComponent& hc = m_worldScene.getEntityManager().getComponent<ska::HitboxComponent>(m_playerId);
			ska::PositionComponent& opponentPc = m_worldScene.getEntityManager().getComponent<ska::PositionComponent>(m_opponentId);
			ska::HitboxComponent& opponentHc = m_worldScene.getEntityManager().getComponent<ska::HitboxComponent>(m_opponentId);

			/* Création d'une entité : Pokéball (Position + Pokeball) */
			pokeball = m_worldScene.getEntityManager().createEntity();
			PokeballComponent pokeballc;
			/* TODO random position autour de l'ennemi */
			pokeballc.finalPos = { (int)(opponentPc.x + opponentHc.xOffset + opponentHc.width / 2), (int)(opponentPc.y + opponentHc.yOffset + opponentHc.height / 2) };
			m_worldScene.getEntityManager().addComponent<PokeballComponent>(pokeball, pokeballc);
			ska::PositionComponent pokePc;
			pokePc = pc;
			pokePc.x += hc.xOffset + hc.width/2;
			pokePc.y += hc.yOffset + hc.height/2;
			m_worldScene.getEntityManager().addComponent<ska::PositionComponent>(pokeball, pokePc);
			return true;
		}
		return m_worldScene.getEntityManager().hasComponent<PokeballComponent>(pokeball)/*wScreen.getPokeball().isVisible()*/;
	}, *dialogRawTask));

	ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<>>* finalRawTask;
	ska::RunnablePtr finalTask = ska::RunnablePtr(finalRawTask = new ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<>>([&](ska::Task<bool, ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<>>& t, ska::InputComponent ic) {
		
		/* Ajout InputComponent au Pokémon (m_opponentId),
		   Ajout d'un IAMovementComponent au dresseur (m_player),
		   Ajout d'un composant de combat au Pokémon */
		m_worldScene.getEntityManager().addComponent<ska::InputComponent>(m_playerId, ic);
		return false;
	}, *pokeballRawTask));

	WGameCore& wScreen = WGameCore::getInstance();
	wScreen.addTaskToQueue(dialogTask);
	wScreen.addTaskToQueue(pokeballTask);
	wScreen.addTaskToQueue(finalTask);
	
}

void SceneFight::unload() {
	m_worldScene.unload();
}

void SceneFight::eventUpdate(bool movingDisallowed) {
	/*WGameCore& core = WGameCore::getInstance();
	Fight& fight = core.getFight();*/
	AbstractSceneMap::eventUpdate(movingDisallowed);
	m_worldScene.eventUpdate(movingDisallowed);

	//Evénements combat
	/*fight.refreshFight();*/
}

SceneFight::~SceneFight()
{
}
