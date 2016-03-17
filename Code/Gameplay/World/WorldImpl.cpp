#include <list>
#include "WorldImpl.h"
#include "../WGameCore.h"
#include "../../ska/Graphic/Draw/DrawableContainer.h"
#include "../../ska/Physic/ParticleManager.h"
#include "../../ska/Physic/MovementComponent.h"
#include "../../ska/Graphic/GraphicComponent.h"
#include "../../ska/Inputs/InputComponent.h"
#include "../../ska/Physic/ForceComponent.h"
#include "../../Utils/IDs.h"
#include "../../ska/World/Layer.h"
#include "../../ska/World/LayerE.h"
#include "../../ska/Graphic/SpritePath.h"

WorldImpl::WorldImpl(const unsigned int tailleBloc, const unsigned int wWidth, const unsigned int wHeight) : ska::World(tailleBloc, wWidth, wHeight), 
m_collisionSystem(*this, m_entityManager), m_movementSystem(m_entityManager), m_graphicSystem(m_cameraSystem, m_entityManager), m_gravitySystem(m_entityManager),
m_forceSystem(m_entityManager) {
}

void WorldImpl::graphicUpdate(ska::DrawableContainer& drawables) {
	ska::Rectangle rectAnimBlocks = m_animBlocks.getRectOfCurrentFrame();
	WGameCore& wScreen = WGameCore::getInstance();

	//Liste de tous les personnages sur le monde courant
	//list<Character*>& currentEntityList = wScreen.getEntityFactory().getCharacterList();

	//Première couche
	drawables.addHead(*m_lBot);

	//Deuxième couche
	drawables.addHead(*m_lMid);

	//Affichage des effets
	ska::ParticleManager& particleManager = wScreen.getParticleManager();
	drawables.addHead(particleManager);

	//Curseur souris sur la map
	drawables.addHead(wScreen.getMouseCursor());

	m_graphicSystem.setDrawables(drawables);
	m_graphicSystem.refresh();
	
	//Troisième couche
	drawables.addHead2D(*m_lTop);
}

void WorldImpl::load(std::string fileName, std::string chipsetName, std::string saveName) {
	World::load(fileName, chipsetName, saveName);
	ska::Point<int> posEntityId;

	//Suppression des anciennes entités
	//wScreen.getEntityFactory().deleteAll();


	//Character* hero;
	unsigned int startPosx, startPosy;
	ska::IniReader reader("."FILE_SEPARATOR"Data"FILE_SEPARATOR"Saves"FILE_SEPARATOR + saveName + FILE_SEPARATOR"trainer.ini");
	WGameCore& wScreen = WGameCore::getInstance();

	startPosx = reader.getInt("Trainer start_posx");
	startPosy = reader.getInt("Trainer start_posy");
	std::string startMapName = reader.getString("Trainer start_map_name");

	string buf = "."FILE_SEPARATOR"Levels"FILE_SEPARATOR;
	buf += startMapName;
	buf += FILE_SEPARATOR;
	buf += startMapName;
	buf += ".ini";

	ska::IniReader mapReader(buf);
	std::string startMapChipset = mapReader.getString("Chipset file");

	if (startMapChipset == "STRINGNOTFOUND")
		cerr << "Erreur : impossible de trouver le nom du chipset de la map de depart" << endl;

	/*hero = wScreen.getEntityFactory().getTrainer();
	hero->teleport(startPosx*TAILLEBLOC, startPosy*TAILLEBLOC);*/

	ska::EntityId hero = m_entityManager.createEntity();
	ska::PositionComponent pc;
	pc.x = startPosx*TAILLEBLOC;
	pc.y = startPosy*TAILLEBLOC;
	pc.z = 0;
	m_entityManager.addComponent<ska::PositionComponent>(hero, pc);
	m_entityManager.addComponent<ska::CameraFocusedComponent>(hero, ska::CameraFocusedComponent());
	ska::InputComponent ic;
	ic.movePower = 2;
	m_entityManager.addComponent<ska::InputComponent>(hero, ic);
	ska::MovementComponent mc;
	memset(&mc, 0, sizeof(ska::MovementComponent));
	ska::ForceComponent fc;
	memset(&fc, 0, sizeof(fc));
	ska::GravityAffectedComponent gac;
	gac.friction = 8.5;
	gac.weight = 65;
	m_entityManager.addComponent<ska::GravityAffectedComponent>(hero, gac);
	m_entityManager.addComponent<ska::ForceComponent>(hero, fc);
	m_entityManager.addComponent<ska::MovementComponent>(hero, mc);
	ska::GraphicComponent gc;
	gc.sprite.load(ska::SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, 0), 6, 8, 3);
	gc.sprite.setDelay(300);
	m_entityManager.addComponent<ska::GraphicComponent>(hero, gc);
	ska::HitboxComponent hc;
	hc.xOffset = 0;
	hc.yOffset = gc.sprite.getHeight() / 2;
	hc.height = hc.yOffset;
	hc.width = gc.sprite.getWidth();
	m_entityManager.addComponent<ska::HitboxComponent>(hero, hc);


	//Chargement des NPC sur la map (personnages & pokémon)
	for (int i = 1; i < m_lEvent->getNbrLignes(); i++)
	{
		posEntityId.y = m_lEvent->getBlocY(i) * TAILLEBLOC;
		posEntityId.x = m_lEvent->getBlocX(i) * TAILLEBLOC;
		int id = m_lEvent->getID(i);
		if (id == 0) {
			continue;
		}

		if (abs(id) <= ENTITEMAX) {	
			ska::EntityId newEntity = m_entityManager.createEntity();
			ska::PositionComponent pc;
			pc.x = posEntityId.x;
			pc.y = posEntityId.y;
			pc.z = 0;
			m_entityManager.addComponent<ska::PositionComponent>(newEntity, pc);
			ska::MovementComponent mc;
			memset(&mc, 0, sizeof(mc));
			m_entityManager.addComponent<ska::MovementComponent>(newEntity, mc);
			ska::GraphicComponent gc;
			gc.sprite.load(ska::SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, id), 6, 8, 3);
			m_entityManager.addComponent<ska::GraphicComponent>(newEntity, gc);

			//m_entityManager.addNPC(id, posEntityId, m_lEvent->getPath(i));
		} else {
			cerr << "Erreur (fonction LoadEntities) : Impossible de lire l'ID de l'entité ligne " << i << endl;
		}

	}

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
