#include "Game/__internalConfig/LoggerConfig.h"
#include "CharsetSheet.h"
#include "Graphic/AnimatedTexture.h"
#include "Core/Utils/SpritePath.h"
#include "Core/Utils/SkaConstants.h"

void Charset::load(ska::AnimatedTexture& texture, int id) {
	const auto path = ska::SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, id);
	texture.load(path, Columns, 8, 3);
	// Square frames : the columns of the sheet from its height
	if (texture.getFullHeight() > 0 && texture.getFullWidth() * 8 == texture.getFullHeight() * GaitColumns) {
		texture.load(path, GaitColumns, 8, 3);
	}
}

bool Charset::hasGaits(const ska::AnimatedTexture& texture) {
	return texture.getWidth() > 0 && texture.getFullWidth() / texture.getWidth() >= GaitColumns;
}
