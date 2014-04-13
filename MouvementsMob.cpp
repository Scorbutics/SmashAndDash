#include <fstream>
#include <vector>
#include <memory>

#include "IDs.h"
#include "MouvementsMob.h"
#include "ChargementImages.h"
#include "Scrolling.h"
#include "WGameCore.h"

using namespace std;



void MobMovements()
{
	/////////////////////
	//Evenements       //
	//automatiques     //
	/////////////////////

	WGameCore& wScreen = WGameCore::getInstance();
	list<Character*>& currentListEntities = wScreen.getEntityFactory().getNPCList();

	for (Character* npc : currentListEntities)
	{
		if(npc != NULL && !npc->getPath()->isMotionless())
		{
			npc->setCountD(npc->getCountD() - 1);

			int direction = npc->getDirection();
			if(npc->getCountD() <= 0)
			{

				if(npc->getPath()->isRandom())
					npc->setDirection(rand()%(20-0) +0); //chiffre aléatoire entre 0 et 19
				else if(npc->getPath()->isPredefined())
					npc->setDirection(npc->getPath()->getNextPathDirection());


				direction = npc->getDirection();
				npc->setCountD(30);

				npc->applyForce(direction, (float)(npc->getSpeedLimit()*5*8));
			}

		}
	}




	if(wScreen.getFight().isFighting()) //Si on est en combat, on gère les mouvements de l'adversaire
	{
		Character* opponent = wScreen.getFight().getOpponent();
		if (opponent != NULL)
		{
			opponent->setCountD(opponent->getCountD() - 1);

			if (opponent->getCountD() <= 0)
			{
				int direction = opponent->getPath()->getNextPathDirection();
				if (direction != -1)
					opponent->setDirection(direction);


				opponent->setCountD(10);
				//int direction = opponent->getDirection();


				opponent->applyForce(opponent->getDirection(), (float)(opponent->getSpeedLimit() * 5 * 8));
			}
		}

		Character* trainer = wScreen.getEntityFactory().getTrainer();


		//mouvements aléatoires (pour l'instant) du dresseur durant un combat
		trainer->setCountD(trainer->getCountD() - 1);
		if(trainer->getCountD() <= 0)
		{
			int direction, r = rand()%4;
			trainer->setCountD(30);
			SDL_Rect posTrainer = trainer->getPos(), posPkmn = wScreen.getFight().getPokemon()->getPos();

			if(r == 0)
				direction = GetDirectionFromPos(&posPkmn, &posTrainer) + 1;
			else if(r == 1)
				direction = GetDirectionFromPos(&posPkmn, &posTrainer) - 1;
			else
				return;

			trainer->applyForce(direction, (float)(trainer->getSpeedLimit()*5*8));

		}
	}


}

