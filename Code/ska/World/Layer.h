#ifndef DEF_MONDE
#define DEF_MONDE

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

#include "Block.h"

#include "../Graphic/Draw/DrawableFixedPriority.h"

class ParticleManager;


namespace ska {
	class World;
	class Layer : public DrawableFixedPriority
	{
	public:
		Layer(ska::World& world, std::string nomFichier, std::string chipsetName, ska::Layer* parent = NULL);
		~Layer();

		void changeLevel(std::string fname, std::string chipsetname);
		void reset(std::string file, std::string chipsetName);

		void printCollisionProfile();
		void clear();

		void display() override;
		bool isVisible() const override;

		Block* getBlock(unsigned int i, unsigned int j);
		int getBlockCollision(const unsigned int i, const unsigned int j);
		std::string getWorldName();
		int getNbrBlocX();
		int getNbrBlocY();
		std::string getChipsetName();
		void getData();
		Layer* getParent() const;

		void setRectAnim(ska::Rectangle rectAnim);

	private:
		void checkSize(int nbrBlocX, int nbrBlocY);

		Layer* m_parent;
		World& m_world;
		std::string m_name, m_nomFichier, m_chipsetname;
		std::vector<std::vector<BlockPtr>> m_block;
		ska::Rectangle m_rectAnim;
		int m_seed, m_type;
		int m_fileWidth, m_fileHeight;

	};
	typedef std::unique_ptr<Layer> LayerPtr;

	
}


#endif
