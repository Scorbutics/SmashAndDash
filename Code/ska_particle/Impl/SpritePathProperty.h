#pragma once
#include <string>

namespace ska {
	class ParticleGroup;

	class SpritePathProperty {
	public:
		~SpritePathProperty() = default;

		static void applyProperty(ParticleGroup& group, const std::string& spritePath);
	private:
		SpritePathProperty() = default;
	};
}