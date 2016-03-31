#include "../World/WorldScene.h"
#include "SceneMap.h"


SceneMap::SceneMap(ska::SceneHolder& sh, ska::InputContextManager& ril, WorldScene& ws, const std::string fileName, const std::string chipsetName) :
AbstractSceneMap(sh, ril),
m_fileName(fileName),
m_chipsetName(chipsetName),
m_worldScene(ws),
m_iaMovementSystem(ws.getEntityManager()),
m_mobSpawningSystem(ws, ws.getEntityManager(), 15000),
m_scriptAutoSystem(ws.getEntityManager(), ws.getSaveGame()),
m_scriptSystem(m_scriptAutoSystem, ril, ws.getWorld().getBlockSize(), ws.getEntityManager()),
m_fightStartSystem(sh, ws, ril, ws.getPlayer()){
	m_logics.push_back(&m_scriptSystem);
	m_logics.push_back(&m_iaMovementSystem);
	m_logics.push_back(&m_mobSpawningSystem);
}

SceneMap::SceneMap(ska::SceneHolder& sh, ska::InputContextManager& ril, WorldScene& ws) : SceneMap(sh, ril, ws, ws.getSaveGame().getStartMapName(), ws.getSaveGame().getStartChipsetName()) {

}

void SceneMap::graphicUpdate(ska::DrawableContainer& drawables) {
	AbstractSceneMap::graphicUpdate(drawables);
	m_worldScene.graphicUpdate(drawables);
}

void SceneMap::load() {
	reinit();
}

void SceneMap::unload() {

}

void SceneMap::reinit() {
	bool firstTime = !m_worldScene.loadedOnce();
	if (firstTime) {
		/* Do not delete the player between 2 maps, just TP it */
		std::unordered_set<ska::EntityId> toNotDelete;
		toNotDelete.insert(m_worldScene.getPlayer());

		/* Delete others entities */
		m_worldScene.getEntityManager().removeEntities(toNotDelete);
	}

	m_scriptSystem.clearNamedScriptedEntities();
	std::unordered_map<std::string, ska::EntityId>& entities = m_worldScene.reinit(m_fileName, m_chipsetName);
	for (auto& e : entities) {
		m_scriptSystem.registerNamedScriptedEntity(e.first, e.second);
	}

	/* Player Script ID = 0 */
	m_scriptSystem.registerNamedScriptedEntity("0", m_worldScene.getPlayer());
}

void SceneMap::eventUpdate(bool stuck) {
	AbstractSceneMap::eventUpdate(stuck);

	/*WGameCore& core = WGameCore::getInstance();
	//MobSpawningManager& mobSpawner = core.getMobSpawningManager();
	Fight& fight = core.getFight();*/

	//gère l'apparition aléatoire de Pokémon	
	//mobSpawner.refresh();

	//Evénements de lancement de combat	
	//fight.refresh();

	/*WGameCore& core = WGameCore::getInstance();
	World& world = core.getWorld();
	Fight& fight = core.getFight();
	ParticleManager& particleManager = core.getParticleManager();
	GUI& gui = core.getGUI();
	MobSpawningManager& mobSpawner = core.getMobSpawningManager();
	ShakerManager& shaker = core.getShakerManager();
	InputListener& kdListener = core.getInputListener();*/
	
}

SceneMap::~SceneMap()
{
}
