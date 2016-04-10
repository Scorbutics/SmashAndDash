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


typedef std::unique_ptr<DialogMenu> DialogMenuPtr;

SceneFight::SceneFight(ska::SceneHolder& sh, WorldScene& ws, ska::InputContextManager& ril, ska::Point<int> fightPos, FightComponent fc) :
AbstractSceneMap(sh, ril),
m_worldScene(ws),
m_cameraSystem(ws.getEntityManager(), ws.getScreenW(), ws.getScreenH(), fightPos),
m_scriptId(fc.scriptId),
m_level(fc.level),
m_player(fc.fighterPlayer),
m_opponentId(fc.fighterOpponent),
m_opponent("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Monsters"FILE_SEPARATOR + ska::StringUtils::intToStr(m_scriptId) + ".ini") {
	m_logics.push_back(&m_cameraSystem);
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

void SceneFight::load() {
	m_worldScene.linkCamera(&m_cameraSystem);
	m_worldScene.load();

	//TODO stats
	/*m_stats = unique_ptr<Statistics>(new Statistics(&data, "BaseStats"));
	m_stats->nextLevel();
	m_stats->nextLevel();

	refreshStats(true);*/
	std::vector<std::string> skills;
	for (unsigned int i = 0; m_opponent.get("Skills " + ska::StringUtils::intToStr(i)); i++) {
		if (m_opponent.getInt("Skills " + ska::StringUtils::intToStr(i) + "_level") <= m_level) {
			skills.push_back(m_opponent.getString("Skills " + ska::StringUtils::intToStr(i)));
		}
	}

	m_descriptor.load(m_opponent, "Description");
	
	
	int delay = 4000;

	ska::RepeatableTask<ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>* dialogRawTask;
	ska::RunnablePtr dialogTask = ska::RunnablePtr(dialogRawTask = new ska::RepeatableTask<ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>([&, delay](ska::Task<bool, ska::TaskReceiver<>, ska::TaskSender<ska::InputComponent>>& t) {
		static bool started = false;
		static int dialogId = 0;
		if (!started) {
			started = true;

			if (!m_worldScene.getEntityManager().hasComponent<ska::InputComponent>(m_player)) {
				return false;
			}

			ska::InputComponent ic = m_worldScene.getEntityManager().getComponent<ska::InputComponent>(m_player);
			m_worldScene.getEntityManager().removeComponent<ska::InputComponent>(m_player);

			ska::PositionComponent& pc = m_worldScene.getEntityManager().getComponent<ska::PositionComponent>(m_player);
			DialogComponent dc(DialogMenu("Un " + m_descriptor.getName() + " sauvage apparaît !", { 0, 0, TAILLEBLOCFENETRE * 10, TAILLEBLOCFENETRE * 2 }, delay, false));
			dc.dialog.hide(false);

			m_worldScene.getEntityManager().addComponent<DialogComponent>(m_player, dc);
			t.forward(ic);
			return true;
		}
		// Continue until dialog is still visible
		return m_worldScene.getEntityManager().hasComponent<DialogComponent>(m_player);
	}));

	ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<ska::InputComponent>>* pokeballRawTask;
	ska::RunnablePtr pokeballTask = ska::RunnablePtr(pokeballRawTask = new ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<ska::InputComponent>>([&](ska::Task<bool, ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<ska::InputComponent>>& t, ska::InputComponent ic) {
		static bool started = false;
		if (!started) {
			started = true;
			t.forward(ic);

			ska::PositionComponent& pc = m_worldScene.getEntityManager().getComponent<ska::PositionComponent>(m_player);
			ska::PositionComponent& opponentPc = m_worldScene.getEntityManager().getComponent<ska::PositionComponent>(m_opponentId);
			/* Création d'une entité : Pokéball (Position + Graphics) 
			   Ajout d'un composant particulier : PokeballMoveComponent 
			   Ajout d'un système déplaçant la Pokéball */
			//wScreen.getPokeball().launch({pc.x, pc.y}, {opponentPc.x, opponentPc.y}, PokeballLaunchReason::Throw);
			return true;
		}
		return false /*wScreen.getPokeball().isVisible()*/;
	}, *dialogRawTask));

	ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<>>* finalRawTask;
	ska::RunnablePtr finalTask = ska::RunnablePtr(finalRawTask = new ska::RepeatableTask<ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<>>([&](ska::Task<bool, ska::TaskReceiver<ska::InputComponent>, ska::TaskSender<>>& t, ska::InputComponent ic) {
		
		/* Ajout InputComponent au Pokémon (m_opponentId),
		   Ajout d'un IAMovementComponent au dresseur (m_player),
		   Ajout d'un composant de combat au Pokémon */
		m_worldScene.getEntityManager().addComponent<ska::InputComponent>(m_player, ic);
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
