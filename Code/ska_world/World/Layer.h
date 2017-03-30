#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

#include "Block.h"
#include "LayerRenderable.h"

#include "Draw/DrawableFixedPriority.h"

class ParticleManager;


namespace ska {
	class World;
	class Layer {
	public:
		Layer(World& world, std::string nomFichier, std::string chipsetName, Layer* parent = nullptr);
		Layer(World& w, Layer* parent = nullptr);
		void operator=(const Layer&) = delete;
		~Layer() = default;

		LayerRenderable& getRenderable();

		void changeLevel(std::string fname, std::string chipsetname);
		void reset(std::string file, std::string chipsetName);

		void clear();

		Block* getBlock(unsigned int i, unsigned int j) const;
		int getBlockCollision(const unsigned int i, const unsigned int j) const;
		int getNbrBlocX();
		int getNbrBlocY();
		void getData();
		Layer* getParent() const;


	private:
		void checkSize(unsigned int nbrBlocX, unsigned int nbrBlocY) const;

		Layer* m_parent;
		World& m_world;
		std::string m_name, m_nomFichier;
		std::vector<std::vector<Block*>> m_block;
		LayerRenderable m_renderable;
		unsigned int m_fileWidth, m_fileHeight;

	};
	typedef std::unique_ptr<Layer> LayerPtr;


}

