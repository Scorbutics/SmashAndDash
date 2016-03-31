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
#include "../../ska/Utils/StringUtils.h"
#include "../../ska/Utils/RectangleUtils.h"
#include "../../ska/Inputs/Readers/IniReader.h"

WorldImpl::WorldImpl(const unsigned int tailleBloc, const unsigned int wWidth, const unsigned int wHeight) :
ska::World(tailleBloc, wWidth, wHeight)
/**/
 {
}

void WorldImpl::graphicUpdate(ska::DrawableContainer& drawables) {
	//Premi�re couche
	drawables.addHead(*m_lBot);

	//Deuxi�me couche
	drawables.addHead(*m_lMid);

	//Affichage des effets
	/*ska::ParticleManager& particleManager = wScreen.getParticleManager();
	drawables.addHead(particleManager);

	//Curseur souris sur la map
	drawables.addHead(wScreen.getMouseCursor());*/
	
	//Troisi�me couche
	drawables.addHead2D(*m_lTop);
}

void WorldImpl::load(std::string fileName, std::string chipsetName) {
	World::load(fileName, chipsetName);
}

void WorldImpl::changeLevel(std::string fileName, std::string chipsetName) {
	World::changeLevel(fileName, chipsetName);
}


WorldImpl::~WorldImpl() {
}
