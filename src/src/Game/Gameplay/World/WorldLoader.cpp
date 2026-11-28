#include "WorldLoader.h"
#include "Base/IO/Files/FileUtils.h"
#include "World/LayerEventLoaderText.h"
#include "../../Utils/IDs.h"

ska::TileWorldLoaderAggregate BuildWorldLoader(const ska::TilesetCorrespondanceMapper& mapper, const std::string& levelName,
	ska::SDLSurface ground, ska::SDLSurface middle, ska::SDLSurface top) {
	const auto levelFileName = ska::FileNameData{ levelName };

	auto loaders = std::vector<std::unique_ptr<ska::LayerLoader>>{};
	loaders.push_back(std::make_unique<ska::LayerLoaderImage>(mapper, std::move(ground)));
	loaders.push_back(std::make_unique<ska::LayerLoaderImage>(mapper, std::move(middle)));
	loaders.push_back(std::make_unique<ska::LayerLoaderImage>(mapper, std::move(top)));

	auto eventLoaders = std::vector<std::unique_ptr<ska::LayerEventLoader>>{};
	eventLoaders.push_back(std::make_unique<ska::LayerEventLoaderText>(levelName + "/" + levelFileName.name + "E.txt"));

	return ska::TileWorldLoaderAggregate(
		levelName,
		2,
		std::move(loaders),
		std::move(eventLoaders),
		TopLayerPriority);
}

void MarkWater(ska::Tileset& tileset) {
	for (auto y = 0u; y < 4u; y++) {
		for (auto x = 0u; x < 6u; x++) {
			tileset.setPropertiesBitMask(x, y, ska::TilePropertiesType::TILE_PROPERTY_WATER);
		}
	}

	for (auto y = 4u; y < 8u; y++) {
		for (auto x = 0u; x < 3u; x++) {
			tileset.setPropertiesBitMask(x, y, ska::TilePropertiesType::TILE_PROPERTY_WATER);
		}
	}
}

bool IsWater(const ska::Tile& tile) {
	return (tile.properties.bitMask & ska::TilePropertiesType::TILE_PROPERTY_WATER) == ska::TilePropertiesType::TILE_PROPERTY_WATER;
}
