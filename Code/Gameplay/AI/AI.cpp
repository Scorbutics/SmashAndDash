/*
#include "AI.h"
#include "../../Utils\IDs.h"
#include "..\WGameCore.h"
#include "../../Utils\ChargementImages.h"
#include "../../Gameplay/Fight/Skill.h"
#include "../../ska/Utils/RectangleUtils.h"


AI::AI()
{
    m_context = AI_BALANCED; //macro désignant une intelligence artificielle équilibrée
	m_pMove = (float)0.8;
	m_pAttack = (float)0.1;
	m_trainerTimer = 3000;
	m_trainerT0 = SDL_GetTicks();
}


void AI::setContext(unsigned int c)
{
    m_context = c;
    switch(m_context)
    {
        case AI_OFFENSIVE:
			m_pMove = (float)0.8;
			m_pAttack = (float)0.5;
        break;

        case AI_DEFENSIVE:
			m_pMove = (float)0.5;
			m_pAttack = (float)0.1;
        break;

        default:
			m_pMove = (float)0.8;
			m_pAttack = (float)0.1;
        break;
    }
}

unsigned int AI::getContext()
{
    return m_context;
}


Character* AI::getOpponent()
{
	//WGameCore& wScreen = WGameCore::getInstance();
	return NULL;//wScreen.getFight().getOpponent();
}

void AI::act(Fight& fight)
{
	WGameCore& wScreen = WGameCore::getInstance();
	ska::World& w = wScreen.getWorld();

    if(fight.getOpponent() == NULL || !fight.isFighting())
        return;

    int pathProbInt = rand()%2, randBuf = rand()%100;
    char pathChar[2] = "";
    vector<Skill_ptr> *vSkill = fight.getOpponent()->getSkills();
	ska::Rectangle opponentPos = fight.getOpponent()->getHitboxCenterPos(), pkmnPos = fight.getPokemon()->getHitboxCenterPos();

    if(((float)randBuf/10) <= m_pAttack*10)
        for(unsigned int i = 0; i < (*vSkill).size(); i++)
            if((*vSkill)[i]->getContext() == AI_BALANCED && (*vSkill)[i]->cooldownOK())
                fight.getOpponent()->launchSkill(i, pkmnPos);	//lancer l'attaque numéro i dans la direction courante du pokémon


	//Mouvements du Pokémon adverse en combat
    if((((float)randBuf)/10) <= m_pMove*10)
        switch(pathProbInt)
        {
            case 0:
				pathChar[0] = GetCharFromDirection(ska::RectangleUtils::getDirectionFromPos(opponentPos, pkmnPos) % 4);
				fight.getOpponent()->getPath()->setPathString(pathChar);
            break;

            case 1:
				pathChar[0] = GetCharFromDirection(rand() % 8);
				fight.getOpponent()->getPath()->setPathString(pathChar);
            break;

            default:
            break;
        }


	Character* trainer = wScreen.getEntityFactory().getTrainer();
	//mouvements aléatoires du dresseur durant un combat
	if ((long)(SDL_GetTicks() - m_trainerT0) >= m_trainerTimer)
	{
		int direction, r = rand() % 4;
		ska::Rectangle posTrainer = trainer->getPos(), posPkmn = wScreen.getFight().getPokemon()->getPos();

		if (r == 0)
			direction = ska::RectangleUtils::getDirectionFromPos(posPkmn, posTrainer) + 1;
		else if (r == 1)
			direction = ska::RectangleUtils::getDirectionFromPos(posPkmn, posTrainer) - 1;
		else
			trainer->getPath()->setPathString("");


		if (r == 0 || r == 1)
		{
			pathChar[0] = GetCharFromDirection(direction % 4);
			trainer->getPath()->setPathString(pathChar);
		}
		m_trainerT0 = SDL_GetTicks();
	}


	ska::Rectangle screenRect;
	ska::Point<int> centerPos;
	centerPos.x = wScreen.getFight().getPokemon()->getHitboxCenterPos().x;
	centerPos.y = wScreen.getFight().getPokemon()->getHitboxCenterPos().y;

    screenRect.x = screenRect.y = 0;
    screenRect.w = wScreen.getWidth();
    screenRect.h = wScreen.getHeight();
    centerPos.x += wScreen.getORel().x;
    centerPos.y += wScreen.getORel().y;

	if (!ska::RectangleUtils::isPositionInBox(centerPos, screenRect) || !wScreen.getFight().getOpponent()->isAlive()) {
		fight.end(EndFightReason::Win);
	}
}*/

