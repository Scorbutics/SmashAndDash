#pragma once

#include "World/TileWorldLoaderAggregate.h"
#include "World/LayerLoaderImage.h"
#include "World/LayerEventLoaderText.h"
#include "World/TilesetLoaderImage.h"
#include "World/TilesetEventLoaderText.h"

ska::TileWorldLoaderAggregate BuildWorldLoader(const ska::TilesetCorrespondanceMapper& mapper, const std::string& levelName);