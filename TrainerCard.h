#ifndef DEF_TRAINER_CARD
#define DEF_TRAINER_CARD

class TrainerCard
{
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

#endif
