#ifndef DEF_PLAYER
#define DEF_PLAYER
#include "../ska/Utils\Observer.h"
#include "../ska/Inputs/EventArg.h"
#include "Character.h"

class Player :
	public Character, public ska::Observer
{
	public:
		Player(int id);
		virtual void update(ska::Observable* obs, ska::EventArg* e);
		virtual void refresh();
		virtual ~Player();
};

#endif