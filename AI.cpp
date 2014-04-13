
#include "AI.h"
#include "Skill.h"
#include "IDs.h"
#include "InputListener.h"
#include "ChargementImages.h"

using namespace std;

AI::AI()
{
    m_context = AI_BALANCED; //macro désignant une intelligence artificielle équilibrée
	m_pMove = (float)0.8;
	m_pAttack = (float)0.1;

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
	WGameCore& wScreen = WGameCore::getInstance();
    return wScreen.getFight().getOpponent();
}

void AI::act(Fight& fight)
{
	WGameCore& wScreen = WGameCore::getInstance();
	World& w = wScreen.getWorld();

    if(fight.getOpponent() == NULL || !fight.isFighting())
        return;

    int pathProbInt = rand()%2, randBuf = rand()%100;
    char pathChar[2] = "";
    vector<Skill_ptr> *vSkill = fight.getOpponent()->getSkills();
    SDL_Rect opponentPos = fight.getOpponent()->getHitboxCenterPos(), pkmnPos = fight.getPokemon()->getHitboxCenterPos();

    if(((float)randBuf/10) <= m_pAttack*10)
        for(unsigned int i = 0; i < (*vSkill).size(); i++)
            if((*vSkill)[i]->getContext() == AI_BALANCED && (*vSkill)[i]->cooldownOK())
                fight.getOpponent()->launchSkill(i, pkmnPos);	//lancer l'attaque numéro i dans la direction courante du pokémon

    //clog << intToStr(GetDirectionFromPos(&((*m_fight.getOpponent())->getCenterPos()), &((*m_fight.getPokemon())->getCenterPos()))) << endl;

    if((((float)randBuf)/10) <= m_pMove*10)
        switch(pathProbInt)
        {
            case 0:
            pathChar[0] = GetCharFromDirection(GetDirectionFromPos(&opponentPos, &pkmnPos)%4);
            fight.getOpponent()->getPath()->setPathString(pathChar);
            break;

            case 1:
				fight.getOpponent()->getPath()->setPathString(""/*+GetCharFromDirection(rand() % 8)*/);
            break;

            default:
            //m_fight.getOpponent()->getPath()->set
            //m_fight.getOpponent()->getPath()->setPathString("");
            break;
        }
   //else
    //    m_fight.getOpponent()->getPath()->setPathString("");

    SDL_Rect screenRect, centerPos = wScreen.getFight().getPokemon()->getHitboxCenterPos();
    screenRect.x = screenRect.y = 0;
    screenRect.w = wScreen.getWidth();
    screenRect.h = wScreen.getHeight();
    centerPos.x += wScreen.getORel().x;
    centerPos.y += wScreen.getORel().y;

    if(!IsPositionInBox(&centerPos, &screenRect) || !wScreen.getFight().getOpponent()->isAlive())
        fight.end(true);
}
