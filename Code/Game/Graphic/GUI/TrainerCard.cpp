#include <iostream>
#include <cstdlib>
#include "TrainerCard.h"

TrainerCard::TrainerCard()
{
    this->generateTrainerID();
}

unsigned int TrainerCard::getMoney()
{
    return m_money;
}

void TrainerCard::setMoney(unsigned int x)
{
    m_money = x;
}

unsigned int TrainerCard::getTrainerID()
{
    return m_trainerID;
}

void TrainerCard::generateTrainerID()
{
	/* TODO : ... */
    m_trainerID = rand()%(9999999 - 1000000 + 1) + 1000000;
}

TrainerCard::~TrainerCard()
{
}

