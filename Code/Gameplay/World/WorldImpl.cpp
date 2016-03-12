#include <list>
#include "WorldImpl.h"
#include "../WGameCore.h"
#include "../../ska/Graphic/Draw/DrawableContainer.h"
#include "../../ska/Physic/ParticleManager.h"
#include "../../Utils/IDs.h"
#include "../../ska/World/Layer.h"

WorldImpl::WorldImpl(const unsigned int tailleBloc, ska::CameraSystem& camera) : ska::World(tailleBloc, camera) {
}

void WorldImpl::graphicUpdate(ska::DrawableContainer& drawables) {
	ska::Rectangle rectAnimBlocks = m_animBlocks.getRectOfCurrentFrame();
	WGameCore& wScreen = WGameCore::getInstance();

	//Liste de tous les personnages sur le monde courant
	list<Character*>& currentEntityList = wScreen.getEntityFactory().getCharacterList();

	//Premi�re couche
	drawables.addHead(*m_lBot);

	//Deuxi�me couche
	drawables.addHead(*m_lMid);

	//Affichage des effets
	ska::ParticleManager& particleManager = wScreen.getParticleManager();
	drawables.addHead(particleManager);

	//Curseur souris sur la map
	drawables.addHead(wScreen.getMouseCursor());

	for (Character* npc : currentEntityList) {
		vector<unique_ptr<CharacterDrawable>>& parts = npc->getCharacterParts();

		//Premi�re partie des personnages		
		drawables.add(*parts[0]);

		//Deuxi�me partie des personnages (ceux au sol)
		drawables.add(*parts[1]);
	}

	//Troisi�me couche
	drawables.addHead2D(*m_lTop);
}



void WorldImpl::refreshEntities() {
	WGameCore& wScreen = WGameCore::getInstance();

	//On refresh tous les personnages
	auto it = wScreen.getEntityFactory().getCharacterList().begin();
	while (it != wScreen.getEntityFactory().getCharacterList().end()) {
		Character* npc = (*it);
		if (npc->isVisible() && npc->isAlive()) {
			npc->refresh();
		}

		//Si jamais un personnage n'est plus vivant ou est notre Pok�mon en combat alors que le combat est termin�, on le supprime
		if (!(npc->getEntityNumber() == ID_CURRENT_POKEMON && !wScreen.getFight().isFighting())) {
			it++;
		}
		else {
			it = wScreen.getEntityFactory().getCharacterList().erase(it);
		}

	}
}


WorldImpl::~WorldImpl() {
}
