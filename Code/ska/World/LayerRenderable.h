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
		void reset(std::vector<std::vector<BlockRenderablePtr>>& blocks);

		void clear();

		void display() override;
		bool isVisible() const override;

		BlockRenderablePtr& getBlock(unsigned int i, unsigned int j);
	
		void setRectAnim(ska::Rectangle rectAnim);

	private:
		World& m_world;
		std::vector<std::vector<BlockRenderablePtr>> m_block;

	};
	typedef std::unique_ptr<LayerRenderable> LayerRenderablePtr;


}

