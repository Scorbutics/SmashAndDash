#ifndef DEF_MONDEE
#define DEF_MONDEE

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "../Utils/Refreshable.h"

namespace ska {
	class World;
	class LayerE : public Refreshable {
	public:
		virtual void refresh() override;

		LayerE(ska::World& world, std::string nomFichier);
		int getBlocX(int ligne) const;
		int getBlocY(int ligne) const;
		int getID(int ligne) const;
		std::string getParam(int ligne) const;
		int getTrigger(int ligne) const;
		std::string getAction(int ligne) const;
		int getSolide(int ligne) const;
		int getNbrLignes() const;
		void changeLevel(std::string nomFichier);
		std::string getPath(int ligne) const;

	private:
		std::string m_nomFichier, m_chipsetname;
		int m_nbrLignes;
		std::vector<int> m_vcoordBX, m_vcoordBY, m_vID, m_vtrigger, m_vsolide;
		std::vector<std::string> m_vaction, m_vparam, m_vpath;
		World& m_world;
	};
	using LayerEPtr = std::unique_ptr<LayerE>;
}
#endif
