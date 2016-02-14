#ifndef DEF_AI
#define DEF_AI

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "..\Character.h"
#include "..\Fight\Fight.h"

using namespace std;
typedef unique_ptr<Character> Character_ptr;

class AI
{
	public:
		AI();

		void setContext(unsigned int c);
		void act(Fight& fight);
		unsigned int getContext();
		Character* getOpponent();

	private:
		long m_trainerT0, m_trainerTimer;
		unsigned int m_context; //d�fini le type d'intelligence artificielle
		float m_pAttack, //probabilit� d'attaque par seconde
		m_pMove;  //probabilit� de d�placement par seconde
};

#endif
