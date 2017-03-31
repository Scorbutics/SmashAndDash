/*#include "../../ska/Utils/NumberUtils.h"
#include "RainParticle.h"
#include "RainParticleManager.h"

RainParticle::RainParticle(RainParticleManager& parent, int id, int idSprite, ska::Rectangle pos, double lifetime, double splashTime, float acceleration, bool loop, bool relative) : GravityParticle(idSprite, pos, lifetime, splashTime, acceleration, loop, relative), m_parent(parent), m_id(id)
{
}

ska::Point<int> RainParticle::getOrigin() const {
	WGameCore& wScreen = WGameCore::getInstance();
	ska::Rectangle posBuf;
	posBuf.y = -3 * TAILLEBLOC;

	// On "ajoute" quelques colonnes de blocs à ceux affichés à l'écran pour faire apparaître la pluie aux limites de l'écran
	const unsigned int variant = 20;
	const unsigned int nbrDisplayedBlocksX = (wScreen.getWidth() + 1) / TAILLEBLOC + variant;

	// L'id a pour vocation d'être un numéro de bloc. Il n'a pas besoin d'être compris entre 0
	et le nombre de colonnes de blocs affichées car le modulo permet de normaliser le chiffre.
	const int max = ((m_id % nbrDisplayedBlocksX) - variant/2)*TAILLEBLOC;
	const int min = (((m_id - 1) % nbrDisplayedBlocksX) - variant / 2)*TAILLEBLOC;
	posBuf.x = ska::NumberUtils::random(min, max);
	return posBuf;
}

void RainParticle::refresh() {
	GravityParticle::refresh();
	if (m_state == PARTICLE_STATE_LAUNCHED) {
		m_pos.x += m_parent.getWind();
	}
}

RainParticle::~RainParticle()
{
}*/
