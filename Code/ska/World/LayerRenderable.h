#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

#include "BlockRenderable.h"

#include "../Graphic/Draw/DrawableFixedPriority.h"

class ParticleManager;


namespace ska {
	class World;
	class LayerRenderable : public DrawableFixedPriority {
	public:
		LayerRenderable(ska::World& world);
		~LayerRenderable() = default;

		void changeLevel(std::string fname, std::string chipsetname);
		void reset(std::vector<std::vector<BlockRenderable*>>& block);

		void update();
		void clear();

		void display() const override;
		bool isVisible() const override;

		ska::BlockRenderable* getBlock(unsigned int i, unsigned int j);
	
		void setRectAnim(ska::Rectangle rectAnim);

	private:
		World& m_world;
		std::vector<std::vector<BlockRenderable*>> m_block;

	};
	typedef std::unique_ptr<LayerRenderable> LayerRenderablePtr;


}

