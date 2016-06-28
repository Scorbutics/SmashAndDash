#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

#include "Block.h"
#include "LayerRenderable.h"

#include "../Graphic/Draw/DrawableFixedPriority.h"

class ParticleManager;


namespace ska {
	class World;
	class Layer {
	public:
		Layer(ska::World& world, std::string nomFichier, std::string chipsetName, ska::Layer* parent = nullptr);
		Layer(ska::World& w, Layer* parent = nullptr);
		~Layer() = default;

		ska::LayerRenderable& getRenderable();

		void changeLevel(std::string fname, std::string chipsetname);
		void reset(std::string file, std::string chipsetName);

		void printCollisionProfile();
		void clear();

		BlockPtr& getBlock(unsigned int i, unsigned int j);
		int getBlockCollision(const unsigned int i, const unsigned int j);
		int getNbrBlocX();
		int getNbrBlocY();
		void getData();
		Layer* getParent() const;


	private:
		void checkSize(int nbrBlocX, int nbrBlocY);

		Layer* m_parent;
		World& m_world;
		std::string m_name, m_nomFichier;
		std::vector<std::vector<BlockPtr>> m_block;
		LayerRenderable m_renderable;
		int m_fileWidth, m_fileHeight;

	};
	typedef std::unique_ptr<Layer> LayerPtr;

	
}

