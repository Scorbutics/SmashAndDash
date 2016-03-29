#include <list>
#include "WorldImpl.h"
#include "../WGameCore.h"
#include "../CustomEntityManager.h"
#include "../../ska/Graphic/Draw/DrawableContainer.h"
#include "../../ska/Physic/ParticleManager.h"
#include "../../Utils/IDs.h"
#include "../../ska/World/Layer.h"
#include "../../ska/World/LayerE.h"
#include "../../ska/Graphic/SpritePath.h"
#include "../../ska/Exceptions/CorruptedFileException.h"
#include "../../ska/Exceptions/ScriptSyntaxError.h"
#include "../../ska/Utils/StringUtils.h"
#include "../../ska/Script/ScriptSleepComponent.h"
#include "../../ska/Physic/PositionComponent.h"
#include "../../ska/Utils/RectangleUtils.h"
#include "../../ska/Inputs/Readers/IniReader.h"

WorldImpl::WorldImpl(ska::PrefabEntityManager& entityManager, const unsigned int tailleBloc, const unsigned int wWidth, const unsigned int wHeight) :
ska::World(entityManager, tailleBloc, wWidth, wHeight),
m_movementSystem(entityManager),
m_graphicSystem(m_cameraSystem, entityManager),
m_gravitySystem(entityManager),
m_forceSystem(entityManager),
m_daSystem(entityManager),
m_shadowSystem(m_cameraSystem, entityManager),
m_deleterSystem(entityManager),
m_iaMovementSystem(entityManager),
m_mobSpawningSystem(*this, entityManager, 15000), 
m_collisionSystem(*this, entityManager) {
}

void WorldImpl::graphicUpdate(ska::DrawableContainer& drawables) {
	//ska::Rectangle rectAnimBlocks = m_animBlocks.getRectOfCurrentFrame();

	//Liste de tous les personnages sur le monde courant
	//list<Character*>& currentEntityList = wScreen.getEntityFactory().getCharacterList();

	//Première couche
	drawables.addHead(*m_lBot);

	//Deuxième couche
	drawables.addHead(*m_lMid);

	//Affichage des effets
	/*ska::ParticleManager& particleManager = wScreen.getParticleManager();
	drawables.addHead(particleManager);

	//Curseur souris sur la map
	drawables.addHead(wScreen.getMouseCursor());*/

	m_shadowSystem.setDrawables(drawables);
	m_shadowSystem.refresh();

	m_graphicSystem.setDrawables(drawables);
	m_graphicSystem.refresh();
	
	//Troisième couche
	drawables.addHead2D(*m_lTop);
}

void WorldImpl::load(std::string fileName, std::string chipsetName, std::string saveName) {
	World::load(fileName, chipsetName, saveName);

	ska::IniReader reader("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + saveName + FILE_SEPARATOR"trainer.ini");

	ska::Point<int> startPos;
	startPos.x = reader.getInt("Trainer start_posx");
	startPos.y = reader.getInt("Trainer start_posy");
	std::string startMapName = reader.getString("Trainer start_map_name");

	std::string buf = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR;
	buf += startMapName;
	buf += FILE_SEPARATOR;
	buf += startMapName;
	buf += ".ini";

	ska::IniReader mapReader(buf);
	std::string startMapChipset = mapReader.getString("Chipset file");
	if (startMapChipset == "STRINGNOTFOUND") {
		throw ska::CorruptedFileException("Erreur : impossible de trouver le nom du chipset de la map de depart");
	}

	//Chargement des sprites de l'équipe pokémon
	/*const size_t teamSize = getPokemonManager().getPokemonTeamSize();
	for (unsigned int i = 0; i < teamSize; i++)
	{
		//getPokemonManager().getPokemon(i)->setID(getPokemonManager().getPokemon(i)->getID());
		getPokemonManager().getPokemon(i)->setDirection(0);
	}*/

	WGameCore::getInstance().getScriptSystem().clearNamedScriptedEntities();
	std::unordered_map<std::string, ska::EntityId>& entities = reinit(fileName, chipsetName);
	for (auto& e : entities) {
		WGameCore::getInstance().getScriptSystem().registerNamedScriptedEntity(e.first, e.second);
	}
	m_player = m_entityManager.createTrainer(startPos, getBlockSize());

	/* Player Script ID = 0 */
	WGameCore::getInstance().getScriptSystem().registerNamedScriptedEntity("0", m_player);
}

void WorldImpl::changeLevel(std::string fileName, std::string chipsetName) {
	World::changeLevel(fileName, chipsetName);

	/* Do not delete the player between 2 maps, just TP it */
	std::unordered_set<ska::EntityId> toNotDelete;
	toNotDelete.insert(m_player);

	/* Delete others entities */
	m_entityManager.removeEntities(toNotDelete);

	WGameCore::getInstance().getScriptSystem().clearNamedScriptedEntities();
	std::unordered_map<std::string, ska::EntityId>& entities = reinit(fileName, chipsetName);
	for (auto& e : entities) {
		WGameCore::getInstance().getScriptSystem().registerNamedScriptedEntity(e.first, e.second);
	}

	/* Player Script ID = 0 */
	WGameCore::getInstance().getScriptSystem().registerNamedScriptedEntity("0", m_player);
}

std::unordered_map<std::string, ska::EntityId> WorldImpl::reinit(std::string fileName, std::string chipsetName) {
	ska::Point<int> posEntityId;
	ska::Point<int> startPos;

	std::unordered_map<std::string, ska::EntityId> result;

	const unsigned int blockSize = getBlockSize();

	//Chargement des NPC sur la map (personnages & pokémon)
	for (int i = 0; i < m_lEvent->getNbrLignes(); i++)
	{
		posEntityId.y = m_lEvent->getBlocY(i);
		posEntityId.x = m_lEvent->getBlocX(i);
		int id = m_lEvent->getID(i);
		if (id == 0) {
			continue;
		}

		ska::EntityId script;
		if (id == INT_MIN) {
			script = m_entityManager.createEntity();
			ska::PositionComponent pc;
			pc.x = posEntityId.x * blockSize;
			pc.y = posEntityId.y * blockSize;
			pc.z = 0;
			m_entityManager.addComponent<ska::PositionComponent>(script, pc);

		}
		else {
			if (abs(id) <= ENTITEMAX) {
				script = m_entityManager.createCharacter(posEntityId, id, blockSize);
			}
			else {
				throw ska::CorruptedFileException("Erreur (fonction LoadEntities) : Impossible de lire l'ID de l'entité ligne " + ska::StringUtils::intToStr(i));
			}
		}

		ska::ScriptSleepComponent ssc;

		const std::string params = m_lEvent->getParam(i);
		std::vector<std::string> totalArgs = ska::StringUtils::split(params, ',');
		if (!totalArgs.empty()) {
			ssc.args.reserve(totalArgs.size() - 1);
			for (unsigned int i = 1; i < totalArgs.size(); i++) {
				ssc.args.push_back(ska::StringUtils::trim(totalArgs[i]));
			}
		}
		else {
			throw ska::ScriptSyntaxError("Error while reading a script in the event layer file (l." + ska::StringUtils::intToStr(i) + ") : no arguments supplied to the script cmd");
		}
		ssc.name = ska::StringUtils::trim(totalArgs[0]);
		ssc.context = getName();
		ssc.triggeringType = m_lEvent->getTrigger(i);
		ssc.period = 1000;
		m_entityManager.addComponent<ska::ScriptSleepComponent>(script, ssc);
		result[ska::StringUtils::intToStr(i + 2)] = script;
	}
	return result;
}

int WorldImpl::spawnMob(ska::Rectangle pos, unsigned int rmin, unsigned int rmax, unsigned int nbrSpawns, ska::IniReader* dataSpawn) {

	if (nbrSpawns == 0) {
		return 0;
	}

	std::vector<unsigned int> idBlocks;
	for (unsigned int i = 0; dataSpawn->get("Spawn on_blockid_" + ska::StringUtils::intToStr(i)); i++) {
		idBlocks.push_back(dataSpawn->getInt("Spawn on_blockid_" + ska::StringUtils::intToStr(i)));
	}

	const unsigned int idMob = dataSpawn->getInt("Data id");	
	const unsigned int blockSize = getBlockSize();

	int successfulSpawns = 0;
	float angle = (float)((2 * M_PI* (rand() % 360)) / 360);
	for (unsigned int i = 0; i < nbrSpawns; i++) {
		const unsigned int radius = rmin + rand() % (rmax - rmin + 1);

		ska::Point<int> dest;
		dest.x = (int)(radius*cos(angle) + pos.x);
		dest.y = (int)(radius*sin(angle) + pos.y);
		dest.x = (dest.x / blockSize) * blockSize;
		dest.y = (dest.y / blockSize) * blockSize;

		ska::Rectangle boxWorld, boxDest;
		boxWorld.x = 0;
		boxWorld.y = 0;
		boxWorld.w = getNbrBlocX()*blockSize;
		boxWorld.h = getNbrBlocY()*blockSize;
		boxDest.x = dest.x - radius;
		boxDest.y = dest.y - radius;
		boxDest.h = boxDest.w = 30;

		if (ska::RectangleUtils::isPositionInBox(dest, boxWorld) && canMoveToPos(boxDest)) {
			bool spawnAllowed = true;
			for (unsigned int j = 0; j < idBlocks.size(); j++) {
				const ska::Block* b = getHigherBlock(dest.x / blockSize, dest.y / blockSize);
				if (b != NULL && b->getID() == idBlocks[i]) {
					spawnAllowed = false;
				}
			}


			if (spawnAllowed) {
				int level = rand() % (dataSpawn->getInt("Data level_min") + dataSpawn->getInt("Data level_max") + 1) + dataSpawn->getInt("Data level_min");
				ska::EntityId mob = m_entityManager.createCharacter(ska::Point<int>(dest.x / blockSize, dest.y / blockSize), idMob, blockSize);
				ska::IAMovementComponent iamc;
				iamc.delay = 500;				
				/* Random */
				iamc.type = 1;
				
				m_entityManager.addComponent<ska::IAMovementComponent>(mob, iamc);
				successfulSpawns++;
			}
		}
		angle += (float)((2.0*M_PI) / nbrSpawns);

	}
	return successfulSpawns;
	/*return 1;*/
}

void WorldImpl::refreshEntities() {
	//WGameCore& wScreen = WGameCore::getInstance();
	
	m_forceSystem.refresh();
	m_collisionSystem.refresh();
	m_gravitySystem.refresh();
	m_movementSystem.refresh();
	m_cameraSystem.refresh();
	m_daSystem.refresh();
	m_deleterSystem.refresh();
	m_mobSpawningSystem.refresh();
	m_iaMovementSystem.refresh();

	//On refresh tous les personnages
	/*auto it = wScreen.getEntityFactory().getCharacterList().begin();
	while (it != wScreen.getEntityFactory().getCharacterList().end()) {
		Character* npc = (*it);
		if (npc->isVisible() && npc->isAlive()) {
			npc->refresh();
		}

		//Si jamais un personnage n'est plus vivant ou est notre Pokémon en combat alors que le combat est terminé, on le supprime
		if (!(npc->getEntityNumber() == ID_CURRENT_POKEMON && !wScreen.getFight().isFighting())) {
			it++;
		}
		else {
			it = wScreen.getEntityFactory().getCharacterList().erase(it);
		}

	}*/
}


WorldImpl::~WorldImpl() {
}
