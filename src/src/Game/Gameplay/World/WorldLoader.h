#pragma once

#include "World/TileWorldLoaderAggregate.h"
#include "World/LayerLoaderImage.h"

ska::TileWorldLoaderAggregate BuildWorldLoader(const ska::TilesetCorrespondanceMapper& mapper, const std::string& levelName);