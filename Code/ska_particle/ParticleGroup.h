#pragma once
#include <vector>
#include "../ska_core/Point.h"
#include "../ska_core/Draw/Color.h"
#include "Graphic/Texture.h"

namespace ska {

	struct ParticlePhysicData {
		Point<float> velocity;
		Point<float> acceleration;
	};

	class ParticleGroup {
		friend class ParticleFactory;

	private:
		std::size_t activeIndex;

		const std::size_t groupSize;
		unsigned int particleSize;

	public:
		explicit ParticleGroup(std::size_t gSize) :
			activeIndex(0),
			groupSize(gSize), 
			particleSize(1),
			pos(gSize),
			physics(gSize),
			lifetime(gSize),
			color(gSize),
			startColor(gSize),
			endColor(gSize) { }

		~ParticleGroup() = default;

		const std::size_t& getLength() const {
			return activeIndex;
		}

		void swapParticles(std::size_t index, std::size_t index2) {
			std::swap(pos[index], pos[index2]);
			std::swap(physics[index], physics[index2]);
			std::swap(lifetime[index], lifetime[index2]);
			std::swap(color[index], color[index2]);
			std::swap(startColor[index], startColor[index2]);
			std::swap(endColor[index], endColor[index2]);
		}

		void kill(std::size_t index) {
			if (activeIndex > 0 && index < activeIndex) {
				swapParticles(index, --activeIndex);
			}
		}

		void addIndex(unsigned density) {
			activeIndex += density;
			if (activeIndex >= groupSize) {
				activeIndex = groupSize;
				//std::cout << ("Particle group full. Cannot add another particle") << std::endl;
			}
		}

		//SOA for cache locality
		std::vector<Point<float>> pos;
		std::vector<ParticlePhysicData> physics;
		std::vector<int> lifetime;

		std::vector<Color> color;
		std::vector<Color> startColor;
		std::vector<Color> endColor;

		ska::Texture appearance;
	};
}
