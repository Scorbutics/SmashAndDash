#include <list>
#include "WorldImpl.h"
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

WorldImpl::WorldImpl(const unsigned int tailleBloc, const unsigned int wWidth, const unsigned int wHeight) : ska::World(tailleBloc, wWidth, wHeight), 
m_collisionSystem(*this, m_entityManager), m_movementSystem(m_entityManager), m_graphicSystem(m_cameraSystem, m_entityManager), m_gravitySystem(m_entityManager),
m_forceSystem(m_entityManager), m_daSystem(m_entityManager), m_shadowSystem(m_cameraSystem, m_entityManager), m_deleterSystem(m_entityManager) {
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

std::unordered_map<std::string, ska::EntityId> WorldImpl::load(std::string fileName, std::string chipsetName, std::string saveName) {
	World::load(fileName, chipsetName, saveName);
	ska::Point<int> posEntityId;
	ska::Point<int> startPos;
	
	std::unordered_map<std::string, ska::EntityId> result;

	//Suppression des anciennes entités
	m_entityManager.removeEntities();
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

		} else {
			if (abs(id) <= ENTITEMAX) {
				script = m_entityManager.createCharacter(posEntityId, id, blockSize);
			} else {
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
		result[ska::StringUtils::intToStr(i+2)] = script;
	}
	return result;
	//Chargement des sprites de l'équipe pokémon
	/*const size_t teamSize = getPokemonManager().getPokemonTeamSize();
	for (unsigned int i = 0; i < teamSize; i++)
	{
		//getPokemonManager().getPokemon(i)->setID(getPokemonManager().getPokemon(i)->getID());
		getPokemonManager().getPokemon(i)->setDirection(0);
	}*/
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
