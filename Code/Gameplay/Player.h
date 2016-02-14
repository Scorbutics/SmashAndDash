#include "../Utils\Observer.h"
#include "Character.h"
#ifndef DEF_PLAYER
#define DEF_PLAYER

class Player :
	public Character, public Observer
{
	public:
		Player(int id);
		virtual void update(Observable* obs, EventArg* e);
		virtual void refresh();
		virtual ~Player();
};

#endif