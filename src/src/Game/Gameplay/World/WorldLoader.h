#pragma once

#include "World/TileWorldLoaderAggregate.h"
#include "World/LayerLoaderImage.h"
#include "World/Tileset.h"
#include "Graphic/SDLSurface.h"

// The map of a folder ("<world>/w_5_4") : its layers from their images already read, its events (w_5_4E.txt)
ska::TileWorldLoaderAggregate BuildWorldLoader(const ska::TilesetCorrespondanceMapper& mapper, const std::string& levelName,
	ska::SDLSurface ground, ska::SDLSurface middle, ska::SDLSurface top);

// The collision type of the outline of the water (Levels/surf.miniska)
constexpr unsigned int ChipmunkWaterCollisionType = 1;
// The water tiles of the chipsets : their first rows
void MarkWater(ska::Tileset& tileset);
bool IsWater(const ska::Tile& tile);
