#include "SpritePathProperty.h"
#include "../ParticleGroup.h"

void ska::SpritePathProperty::applyProperty(ParticleGroup& group, const std::string& spritePath) {
	group.appearance.load(spritePath);
}
