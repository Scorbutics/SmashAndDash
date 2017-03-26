#pragma once

class TrainerCard {
public:
    TrainerCard();
    unsigned int getMoney();
    void setMoney(unsigned int x);
    unsigned int getTrainerID();
    void generateTrainerID();
    ~TrainerCard();

private:
    unsigned int m_money, m_trainerID;

};
