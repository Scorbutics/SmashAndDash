#pragma once
#include <array>
#include "../ska_core/Point.h"
#include "../ska_core/Draw/Color.h"
#include "../ska_core/Logging/Logger.h"

namespace ska {

	struct ParticlePhysicData {
		Point<float> velocity;
		Point<float> acceleration;
	};

	class ParticleGroup {
		friend class ParticleFactory;

	private:
		std::size_t activeIndex;
		static constexpr auto Size = 20000;

	public:
		ParticleGroup() :
			activeIndex(0) {
		}

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
			if (activeIndex > 0 && index <= activeIndex) {
				swapParticles(index, activeIndex--);
			}
		}

		void addIndex(unsigned density) {
			activeIndex += density;
			if (activeIndex >= Size) {
				activeIndex = Size - 1;
				//std::cout << ("Particle group full. Cannot add another particle") << std::endl;
			}
		}

		//SOA for cache locality
		std::array<Point<int>, Size> pos;
		std::array<ParticlePhysicData, Size> physics;
		std::array<int, Size> lifetime;

		std::array<Color, Size> color;
		std::array<Color, Size> startColor;
		std::array<Color, Size> endColor;
	};
}
