#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include "../Generation/Png.h"

// The images of a chunk, read and decoded away from the game (a thread, a web worker : plain C++, no engine). The game
// makes its map from them (WorldStreamer).
namespace chunks {
	enum ImageIndex {
		// The chipset : its tiles, their collisions, their properties, their masks (optional)
		Chipset, Collisions, Properties, Mask,
		// The layers of the map
		Ground, Middle, Top,
		// The light of the map (optional)
		Light,
		ImageCount
	};

	// The files of a chunk (an empty path or a file not there : an image with no size, for the optional ones)
	using Paths = std::array<std::string, ImageCount>;
	using Files = std::array<std::vector<std::uint8_t>, ImageCount>;

	struct Images {
		std::array<worldgen::RgbImage, ImageCount> image;
		// Not empty : the chunk could not be read
		std::string error;
	};

	// The files of a map : its folder ("<world>/w_5_4") and its chipset image ("<world>/Chipsets/chipset_w_5_4.png")
	Paths PathsOf(const std::string& mapDirectory, const std::string& mapName, const std::string& chipsetImage);

	Files Read(const Paths& paths);
	// Never throws : Images::error
	Images Decode(const Files& files);
	// A 24 or 32 bits BMP file (not compressed) ; throws std::runtime_error
	worldgen::RgbImage DecodeBmp(const std::vector<std::uint8_t>& file);

	// The files and the images as one block of bytes (the messages of the web worker)
	std::vector<std::uint8_t> Pack(const Files& files);
	Files UnpackFiles(const std::uint8_t* data, std::size_t size);
	std::vector<std::uint8_t> Pack(const Images& images);
	Images UnpackImages(const std::uint8_t* data, std::size_t size);
}
