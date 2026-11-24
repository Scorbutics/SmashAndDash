#pragma once
#include <cstdint>
#include <string>
#include <vector>

// The images of the world generator, plain C++ (no engine) : PNG read (8 bits RGB or RGBA, not interlaced : the atlas
// of the tiles), BMP written (24 bits : the maps and the chipsets, read by the game through SDL_image).
namespace worldgen {
	struct RgbImage {
		int width = 0;
		int height = 0;
		std::vector<std::uint8_t> rgb; // by rows, 3 bytes a pixel

		RgbImage() = default;
		RgbImage(int w, int h, std::uint8_t r = 0, std::uint8_t g = 0, std::uint8_t b = 0);
		std::uint8_t* at(int x, int y) { return &rgb[(static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)) * 3]; }
		const std::uint8_t* at(int x, int y) const { return &rgb[(static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)) * 3]; }
	};

	// The data of a zlib stream (RFC 1950, 1951) ; throws std::runtime_error on a broken stream
	std::vector<std::uint8_t> Inflate(const std::uint8_t* data, std::size_t size);
	// A PNG file (its alpha dropped) ; throws std::runtime_error
	RgbImage DecodePng(const std::vector<std::uint8_t>& file);
	RgbImage ReadPng(const std::string& path);
	// A 24 bits BMP file
	std::vector<std::uint8_t> EncodeBmp(const RgbImage& image);
}
