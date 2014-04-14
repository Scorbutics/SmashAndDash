#ifndef DEF_PLAYER
#define DEF_PLAYER

#include "Character.h"
#include "Observer.h"

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