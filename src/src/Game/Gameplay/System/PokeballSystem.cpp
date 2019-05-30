#include "Game/__internalConfig/LoggerConfig.h"
#include "PokeballSystem.h"
#include "Core/ECS/Basics/Graphic/DeleterComponent.h"

#define POWER 5

PokeballSystem::PokeballSystem(ska::EntityManager& entityManager) :
System(entityManager) {
	m_sprite.load("./Sprites/Fight/pokeball.png", 4, 1, 4);
	m_sprite.setDelay(200);
	m_openPokeball.load("./Sprites/Fight/pokeball-openned.png", 1, 1, 1);
	m_vortex.load("./Sprites/Fight/pokeball-aura.png", 2, 1, 2, false, ska::Texture::DEFAULT_T_RED, ska::Texture::DEFAULT_T_GREEN, ska::Texture::DEFAULT_T_BLUE, 100);
	m_vortex.setDelay(400);
}

void PokeballSystem::refresh(unsigned int) {
	const auto& processed = getEntities();
	for (auto entityId : processed) {
		auto& pokec = m_componentAccessor.get<PokeballComponent>(entityId);
		auto& pc = m_componentAccessor.get<ska::PositionComponent>(entityId);

		if (!pokec.started) {
			throwBall(entityId);
		}

		auto& gc = m_componentAccessor.get<ska::GraphicComponent>(entityId);

		//Si la Pokeball est en l'air
		if ((pc.x > pokec.finalPos.x &&  pokec.sens == 0) || (pc.x < pokec.finalPos.x &&  pokec.sens == 1)) {
			//ska::Rectangle animPos = m_gestionAnim.getRectOfCurrentFrame(), oRel = { 0 };
			if (pc.x < pokec.finalPos.x) {
				pc.x += pokec.speed;
			} else {
				pc.x -= pokec.speed;
			}

			//calcul des coordonnées à l'aide de l'équation de parabole préalablement calculée dans Pokeball::launch
			pc.y = static_cast<int>(pokec.a * pc.x * pc.x + pokec.b * pc.x + pokec.c);
		} else if (!pokec.isInactive && !pokec.isOpenning)  {
			//sinon lorsqu'elle tombe par terre
			pokec.isOpenning = true;

			gc.animatedSprites.clear();
			gc.animatedSprites.push_back(m_openPokeball);

			/* Lorsque la pokeball s'ouvre, on y ajoute un effet temporaire de "vortex" */
			const auto vortexEntity = createEntity();
			ska::DeleterComponent vortexDeleter;
			vortexDeleter.delay = 1500;
			auto vortexPc = pc;
			vortexPc.x -= m_vortex.getWidth() / 2 - m_openPokeball.getWidth() / 2;
			vortexPc.y -= m_vortex.getHeight() / 2 - m_openPokeball.getHeight() / 2;

			m_componentAccessor.add<ska::DeleterComponent>(vortexEntity, std::move(vortexDeleter));
			m_componentAccessor.add<ska::PositionComponent>(vortexEntity, std::move(vortexPc));
			ska::GraphicComponent vortexGc;
			vortexGc.animatedSprites.push_back(m_vortex);
			m_componentAccessor.add<ska::GraphicComponent>(vortexEntity, std::move(vortexGc));

			/* On fini par ajouter un deleter sur cette Pokeball (avec un delai plus élevé que celui du vortex) */
			ska::DeleterComponent deleter;
			deleter.delay = vortexDeleter.delay*2;
			m_componentAccessor.add<ska::DeleterComponent>(entityId, std::move(deleter));
		}

	}
}

void PokeballSystem::throwBall(ska::EntityId entityId) {
	auto& pokec = m_componentAccessor.get<PokeballComponent>(entityId);
	auto& pc = m_componentAccessor.get<ska::PositionComponent>(entityId);

	ska::Point<int> leftPos;
	ska::Point<int> rightPos;
	if (pc.x < pokec.finalPos.x) {
		leftPos = pc;
		rightPos = pokec.finalPos;
		pokec.sens = 1;
	} else {
		rightPos = pc;
		leftPos = pokec.finalPos;
		pokec.sens = 0;
	}

	//pente de la droite passant par la mousePos et la pos centrale du hero
	float p;
	if (rightPos.x != leftPos.x) {
		p = static_cast<float>(rightPos.y - leftPos.y) / (rightPos.x - leftPos.x);
	} else {
		p = 100000;
	}

	//on adapte la vitesse à la distance à parcourir pour éviter de privilégier les lancers de pokeball courts.
	pokec.speed = abs(pokec.finalPos.x - pc.x) / 30 + 1;


	//calcul (au préalable fait sur papier) des coefficients de la parabole en fonction de la position du heros et de l'endroit voulu du lancer.
	//théorique :
	//m_a = - (float) ((p*(xD - xF) + 2*m_power) + 2*sqrt(m_power*(m_power + p*(xD - xF)))) / ((xD - xF)*(xD - xF));


	//experimental (théorème de l'arrangement) :
	pokec.a = -static_cast<float>(5) * ((leftPos.x - rightPos.x + 2 * POWER) + 2 * ska::NumberUtils::squareroot(static_cast<double>(abs(POWER * (POWER + rightPos.x - leftPos.x))))) / ((rightPos.x - leftPos.x)*(rightPos.x - leftPos.x));
	pokec.b = p - pokec.a*(rightPos.x + leftPos.x);
	pokec.c = leftPos.y - pokec.a*leftPos.x*leftPos.x - pokec.b*leftPos.x;


	/* Initialisation logique (états) */
	pokec.started = true;
	pokec.isInactive = false;
	pokec.isOpenning = false;

	/* Initialisation Graphique */
	ska::GraphicComponent gc;
	gc.animatedSprites.push_back(m_sprite);
	//gc.desiredPriority = GUI_DEFAULT_DISPLAY_PRIORITY;
	m_componentAccessor.add<ska::GraphicComponent>(entityId, std::move(gc));
}
