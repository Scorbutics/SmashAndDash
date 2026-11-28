#include <cstring>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include "ChunkImages.h"

namespace {
	std::uint32_t Little(const std::uint8_t* p) {
		return static_cast<std::uint32_t>(p[0]) | (static_cast<std::uint32_t>(p[1]) << 8) | (static_cast<std::uint32_t>(p[2]) << 16) | (static_cast<std::uint32_t>(p[3]) << 24);
	}

	void Append(std::vector<std::uint8_t>& out, std::uint32_t value) {
		for (auto i = 0; i < 4; i++) {
			out.push_back(static_cast<std::uint8_t>(value >> (8 * i)));
		}
	}

	void Append(std::vector<std::uint8_t>& out, const std::vector<std::uint8_t>& bytes) {
		Append(out, static_cast<std::uint32_t>(bytes.size()));
		out.insert(out.end(), bytes.begin(), bytes.end());
	}

	class Reader {
	public:
		Reader(const std::uint8_t* data, std::size_t size) : m_data(data), m_size(size) {}
		std::uint32_t number() {
			return Little(take(4));
		}
		std::vector<std::uint8_t> bytes() {
			const auto size = number();
			const auto* start = take(size);
			return { start, start + size };
		}
	private:
		const std::uint8_t* take(std::size_t size) {
			if (size > m_size - m_pos) {
				throw std::runtime_error{ "chunk message : too short" };
			}
			const auto* result = m_data + m_pos;
			m_pos += size;
			return result;
		}
		const std::uint8_t* m_data;
		std::size_t m_size;
		std::size_t m_pos = 0;
	};

	bool Optional(int index) {
		return index == chunks::Mask || index == chunks::Light;
	}
}

chunks::Paths chunks::PathsOf(const std::string& mapDirectory, const std::string& mapName, const std::string& chipsetImage) {
	const auto layer = mapDirectory + "/" + mapName;
	return { chipsetImage, chipsetImage + ".col", chipsetImage + ".prop", chipsetImage + ".mask",
		layer + ".bmp", layer + "M.bmp", layer + "T.bmp", layer + "L.png" };
}

chunks::Files chunks::Read(const Paths& paths) {
	auto files = Files{};
	for (auto i = 0; i < ImageCount; i++) {
		auto file = std::ifstream{ paths[i], std::ios::binary };
		if (file) {
			files[i].assign(std::istreambuf_iterator<char>{ file }, std::istreambuf_iterator<char>{});
		}
	}
	return files;
}

worldgen::RgbImage chunks::DecodeBmp(const std::vector<std::uint8_t>& file) {
	if (file.size() < 54 || file[0] != 'B' || file[1] != 'M') {
		throw std::runtime_error{ "bmp : not a BMP file" };
	}
	const auto offset = static_cast<std::size_t>(Little(&file[10]));
	const auto width = static_cast<std::int32_t>(Little(&file[18]));
	const auto signedHeight = static_cast<std::int32_t>(Little(&file[22]));
	const auto height = signedHeight < 0 ? -signedHeight : signedHeight;
	const auto bits = file[28] | (file[29] << 8);
	const auto compression = Little(&file[30]);
	// 32 bits : "bit fields" is the usual layout (blue, green, red)
	if ((bits != 24 && bits != 32) || (compression != 0 && !(bits == 32 && compression == 3)) || width <= 0 || height <= 0 || width > 4096 || height > 4096) {
		throw std::runtime_error{ "bmp : only 24 or 32 bits, not compressed" };
	}
	const auto pixel = static_cast<std::size_t>(bits / 8);
	const auto row = (static_cast<std::size_t>(width) * pixel + 3) / 4 * 4;
	if (offset + row * static_cast<std::size_t>(height) > file.size()) {
		throw std::runtime_error{ "bmp : the file ends too soon" };
	}
	auto image = worldgen::RgbImage{ width, height };
	for (auto y = 0; y < height; y++) {
		// The rows from the bottom, when the height is positive
		const auto source = offset + row * static_cast<std::size_t>(signedHeight < 0 ? y : height - 1 - y);
		for (auto x = 0; x < width; x++) {
			const auto* from = &file[source + static_cast<std::size_t>(x) * pixel];
			auto* to = image.at(x, y);
			to[0] = from[2];
			to[1] = from[1];
			to[2] = from[0];
		}
	}
	return image;
}

chunks::Images chunks::Decode(const Files& files) {
	auto result = Images{};
	for (auto i = 0; i < ImageCount; i++) {
		if (files[i].empty()) {
			if (!Optional(i)) {
				result.error = "a file of the chunk is missing (image " + std::to_string(i) + ")";
				return result;
			}
			continue;
		}
		try {
			result.image[i] = i >= Ground && i <= Top ? DecodeBmp(files[i]) : worldgen::DecodePng(files[i]);
		} catch (const std::exception& e) {
			result.error = std::string{ e.what() } + " (image " + std::to_string(i) + ")";
			return result;
		}
	}
	return result;
}

std::vector<std::uint8_t> chunks::Pack(const Files& files) {
	auto out = std::vector<std::uint8_t>{};
	for (const auto& file : files) {
		Append(out, file);
	}
	return out;
}

chunks::Files chunks::UnpackFiles(const std::uint8_t* data, std::size_t size) {
	auto in = Reader{ data, size };
	auto files = Files{};
	for (auto& file : files) {
		file = in.bytes();
	}
	return files;
}

std::vector<std::uint8_t> chunks::Pack(const Images& images) {
	auto out = std::vector<std::uint8_t>{};
	Append(out, std::vector<std::uint8_t>{ images.error.begin(), images.error.end() });
	for (const auto& image : images.image) {
		Append(out, static_cast<std::uint32_t>(image.width));
		Append(out, static_cast<std::uint32_t>(image.height));
		Append(out, image.rgb);
	}
	return out;
}

chunks::Images chunks::UnpackImages(const std::uint8_t* data, std::size_t size) {
	auto in = Reader{ data, size };
	auto images = Images{};
	const auto error = in.bytes();
	images.error.assign(error.begin(), error.end());
	for (auto& image : images.image) {
		image.width = static_cast<int>(in.number());
		image.height = static_cast<int>(in.number());
		image.rgb = in.bytes();
		if (image.rgb.size() != static_cast<std::size_t>(image.width) * static_cast<std::size_t>(image.height) * 3) {
			throw std::runtime_error{ "chunk message : a wrong image" };
		}
	}
	return images;
}
