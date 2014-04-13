#ifndef DEF_MONDEE
#define DEF_MONDEE

#include <iostream>
#include <string>
#include <vector>

class LayerE
{
    public:
    LayerE(std::string nomFichier);
    int getBlocX(int ligne);
    int getBlocY(int ligne);
    int getID(int ligne);
    std::string getParam(int ligne);
    int getTrigger(int ligne);
    std::string getAction(int ligne);
    int getSolide(int ligne);
    int getNbrLignes();
    void changeLevel(std::string nomFichier);
    std::string getPath(int ligne);

    private:
    std::string m_nomFichier,  m_chipsetname;
    int m_nbrLignes;
    std::vector<int> m_vcoordBX, m_vcoordBY, m_vID, m_vtrigger, m_vsolide;
    std::vector<std::string> m_vaction, m_vparam, m_vpath;
};

#endif
