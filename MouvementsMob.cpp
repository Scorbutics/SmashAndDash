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

	/*WGameCore& wScreen = WGameCore::getInstance();
	list<Character*>& currentListEntities = wScreen.getEntityFactory().getNPCList();

	for (Character* npc : currentListEntities)
	{
		if(!npc->getPath()->isMotionless())
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
	}*/


}

