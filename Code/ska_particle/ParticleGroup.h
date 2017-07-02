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
		static constexpr auto Size = 5000;

	public:
		ParticleGroup() :
			activeIndex(0) {
		}

		~ParticleGroup() = default;

		std::size_t getLength() const{
			return activeIndex;
		}

		void kill(std::size_t index) {
			if (activeIndex > 0 && index < activeIndex) {
				std::swap(pos[index], pos[activeIndex]);
				std::swap(physics[index], physics[activeIndex]);
				std::swap(lifetime[index], lifetime[activeIndex]);
				std::swap(color[index], color[activeIndex]);
				std::swap(startColor[index], startColor[activeIndex]);
				std::swap(endColor[index], endColor[activeIndex]);
			}
		}

		void addIndex(unsigned density) {
			activeIndex += density;
			if(activeIndex >= Size) {
				activeIndex = Size - 1;
				SKA_LOG_ERROR("Particle group full. Cannot add another particle");
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
