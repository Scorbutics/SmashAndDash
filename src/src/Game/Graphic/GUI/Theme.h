#pragma once
#include <string>
#include "Core/Rectangle.h"

namespace ska {
	class Renderer;
	class Texture;
}

// The theme of the interface (Menu/default_theme, made by tools/tilegen/ui.py) for the screens that draw themselves
// (the battle) : the same panels as the windows, over any rectangle.
namespace Theme {
	// An image of the theme (cached), ex : "panel_small.png"
	const ska::Texture& image(const std::string& name);

	// A 9-slice image of the theme over a rectangle : its corners ("corner" pixels) as they are, its edges and its centre
	// repeated. "alpha" : its opacity
	void panel(ska::Renderer& renderer, const std::string& name, const ska::Rectangle& box, int corner, unsigned char alpha = 255);
}
