#include "WorldLoader.h"
#include "Utils/FileUtils.h"

ska::TileWorldLoaderAggregate BuildWorldLoader(const ska::TilesetCorrespondanceMapper& mapper, const std::string& levelName) {
	const auto levelFileName = ska::FileNameData{ levelName };

	auto loaders = std::vector<std::unique_ptr<ska::LayerLoader>>{};
	loaders.push_back(std::make_unique<ska::LayerLoaderImage>(mapper, levelName + "/" + levelFileName.name + ".bmp"));
	loaders.push_back(std::make_unique<ska::LayerLoaderImage>(mapper, levelName + "/" + levelFileName.name + "M.bmp"));
	loaders.push_back(std::make_unique<ska::LayerLoaderImage>(mapper, levelName + "/" + levelFileName.name + "T.bmp"));

	auto eventLoaders = std::vector<std::unique_ptr<ska::LayerEventLoader>>{};
	eventLoaders.push_back(std::make_unique<ska::LayerEventLoaderText>(levelName + "/" + levelFileName.name + "E.txt"));

	return ska::TileWorldLoaderAggregate(
		levelName,
		std::move(loaders),
		std::move(eventLoaders));
}
