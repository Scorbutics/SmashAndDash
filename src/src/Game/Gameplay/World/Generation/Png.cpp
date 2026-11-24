#include <algorithm>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include "Png.h"

worldgen::RgbImage::RgbImage(int w, int h, std::uint8_t r, std::uint8_t g, std::uint8_t b) :
	width(w), height(h), rgb(static_cast<std::size_t>(w) * static_cast<std::size_t>(h) * 3) {
	for (auto i = std::size_t{ 0 }; i < rgb.size(); i += 3) {
		rgb[i] = r;
		rgb[i + 1] = g;
		rgb[i + 2] = b;
	}
}

namespace {
	// Inflate (RFC 1951), after zlib's puff : canonical Huffman codes decoded bit by bit
	class Bits {
	public:
		Bits(const std::uint8_t* data, std::size_t size) : m_data(data), m_size(size) {}
		unsigned bits(int need) {
			auto value = static_cast<std::uint64_t>(m_buffer);
			while (m_count < need) {
				value |= static_cast<std::uint64_t>(byte()) << m_count;
				m_count += 8;
			}
			m_buffer = static_cast<std::uint32_t>(value >> need);
			m_count -= need;
			return static_cast<unsigned>(value & ((std::uint64_t{ 1 } << need) - 1));
		}
		void align() { m_buffer = 0; m_count = 0; }
		std::uint8_t byte() {
			if (m_pos >= m_size) {
				throw std::runtime_error{ "inflate : the data ends too soon" };
			}
			return m_data[m_pos++];
		}
		std::size_t pos() const { return m_pos; }
	private:
		const std::uint8_t* m_data;
		std::size_t m_size;
		std::size_t m_pos = 0;
		std::uint32_t m_buffer = 0;
		int m_count = 0;
	};

	struct Huffman {
		std::array<int, 16> count{};
		std::vector<int> symbol;

		Huffman(const int* lengths, int n) : symbol(static_cast<std::size_t>(n)) {
			for (auto s = 0; s < n; s++) {
				count[static_cast<std::size_t>(lengths[s])]++;
			}
			auto offs = std::array<int, 16>{};
			for (auto len = 1; len < 15; len++) {
				offs[static_cast<std::size_t>(len + 1)] = offs[static_cast<std::size_t>(len)] + count[static_cast<std::size_t>(len)];
			}
			for (auto s = 0; s < n; s++) {
				if (lengths[s] != 0) {
					symbol[static_cast<std::size_t>(offs[static_cast<std::size_t>(lengths[s])]++)] = s;
				}
			}
		}

		int decode(Bits& in) const {
			auto code = 0, first = 0, index = 0;
			for (auto len = 1; len < 16; len++) {
				code |= static_cast<int>(in.bits(1));
				const auto n = count[static_cast<std::size_t>(len)];
				if (code - n < first) {
					return symbol[static_cast<std::size_t>(index + (code - first))];
				}
				index += n;
				first = (first + n) << 1;
				code <<= 1;
			}
			throw std::runtime_error{ "inflate : a bad code" };
		}
	};

	constexpr std::array<int, 29> LengthBase{ 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258 };
	constexpr std::array<int, 29> LengthExtra{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0 };
	constexpr std::array<int, 30> DistBase{ 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577 };
	constexpr std::array<int, 30> DistExtra{ 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13 };

	void Codes(Bits& in, std::vector<std::uint8_t>& out, const Huffman& lengths, const Huffman& distances) {
		while (true) {
			const auto symbol = lengths.decode(in);
			if (symbol < 256) {
				out.push_back(static_cast<std::uint8_t>(symbol));
			} else if (symbol == 256) {
				return;
			} else {
				const auto s = static_cast<std::size_t>(symbol - 257);
				if (s >= LengthBase.size()) {
					throw std::runtime_error{ "inflate : a bad length" };
				}
				const auto len = LengthBase[s] + static_cast<int>(in.bits(LengthExtra[s]));
				const auto d = static_cast<std::size_t>(distances.decode(in));
				if (d >= DistBase.size()) {
					throw std::runtime_error{ "inflate : a bad distance" };
				}
				const auto dist = static_cast<std::size_t>(DistBase[d] + static_cast<int>(in.bits(DistExtra[d])));
				if (dist > out.size()) {
					throw std::runtime_error{ "inflate : a distance too far" };
				}
				for (auto i = 0; i < len; i++) {
					out.push_back(out[out.size() - dist]);
				}
			}
		}
	}
}

std::vector<std::uint8_t> worldgen::Inflate(const std::uint8_t* data, std::size_t size) {
	if (size < 2 || (data[0] & 0x0F) != 8 || (data[1] & 0x20) != 0) {
		throw std::runtime_error{ "inflate : not a zlib stream" };
	}
	auto in = Bits{ data + 2, size - 2 };
	auto out = std::vector<std::uint8_t>{};
	auto last = 0u;
	do {
		last = in.bits(1);
		const auto type = in.bits(2);
		if (type == 0) {
			in.align();
			const auto low = in.byte();
			const auto len = low | (in.byte() << 8);
			in.byte();
			in.byte();
			for (auto i = 0; i < len; i++) {
				out.push_back(in.byte());
			}
		} else if (type == 1) {
			static const auto fixed = [] {
				auto lengths = std::array<int, 288>{};
				for (auto s = 0; s < 288; s++) {
					lengths[static_cast<std::size_t>(s)] = s < 144 ? 8 : s < 256 ? 9 : s < 280 ? 7 : 8;
				}
				auto dist = std::array<int, 30>{};
				dist.fill(5);
				return std::make_pair(Huffman{ lengths.data(), 288 }, Huffman{ dist.data(), 30 });
			}();
			Codes(in, out, fixed.first, fixed.second);
		} else if (type == 2) {
			const auto nlen = static_cast<int>(in.bits(5)) + 257;
			const auto ndist = static_cast<int>(in.bits(5)) + 1;
			const auto ncode = static_cast<int>(in.bits(4)) + 4;
			static constexpr std::array<int, 19> order{ 16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15 };
			auto lengths = std::array<int, 320>{};
			for (auto i = 0; i < ncode; i++) {
				lengths[static_cast<std::size_t>(order[static_cast<std::size_t>(i)])] = static_cast<int>(in.bits(3));
			}
			const auto code = Huffman{ lengths.data(), 19 };
			auto i = 0;
			while (i < nlen + ndist) {
				const auto symbol = code.decode(in);
				if (symbol < 16) {
					lengths[static_cast<std::size_t>(i++)] = symbol;
					continue;
				}
				auto value = 0;
				auto repeat = 0;
				if (symbol == 16) {
					if (i == 0) {
						throw std::runtime_error{ "inflate : a repeat with nothing before" };
					}
					value = lengths[static_cast<std::size_t>(i - 1)];
					repeat = 3 + static_cast<int>(in.bits(2));
				} else if (symbol == 17) {
					repeat = 3 + static_cast<int>(in.bits(3));
				} else {
					repeat = 11 + static_cast<int>(in.bits(7));
				}
				if (i + repeat > nlen + ndist) {
					throw std::runtime_error{ "inflate : too many lengths" };
				}
				while (repeat--) {
					lengths[static_cast<std::size_t>(i++)] = value;
				}
			}
			Codes(in, out, Huffman{ lengths.data(), nlen }, Huffman{ lengths.data() + nlen, ndist });
		} else {
			throw std::runtime_error{ "inflate : a bad block" };
		}
	} while (!last);
	return out;
}

namespace {
	std::uint32_t BigEndian(const std::uint8_t* p) {
		return (static_cast<std::uint32_t>(p[0]) << 24) | (static_cast<std::uint32_t>(p[1]) << 16) | (static_cast<std::uint32_t>(p[2]) << 8) | p[3];
	}
}

worldgen::RgbImage worldgen::DecodePng(const std::vector<std::uint8_t>& file) {
	static const std::uint8_t signature[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
	if (file.size() < 8 || !std::equal(signature, signature + 8, file.begin())) {
		throw std::runtime_error{ "png : not a PNG file" };
	}
	auto pos = std::size_t{ 8 };
	auto width = 0, height = 0, channels = 0;
	auto compressed = std::vector<std::uint8_t>{};
	while (pos + 8 <= file.size()) {
		const auto length = BigEndian(&file[pos]);
		const auto type = std::string(reinterpret_cast<const char*>(&file[pos + 4]), 4);
		if (pos + 12 + length > file.size()) {
			throw std::runtime_error{ "png : a chunk ends too soon" };
		}
		const auto* data = &file[pos + 8];
		if (type == "IHDR") {
			width = static_cast<int>(BigEndian(data));
			height = static_cast<int>(BigEndian(data + 4));
			const auto depth = data[8];
			const auto colour = data[9];
			if (depth != 8 || (colour != 2 && colour != 6) || data[12] != 0) {
				throw std::runtime_error{ "png : only 8 bits RGB or RGBA, not interlaced" };
			}
			channels = colour == 2 ? 3 : 4;
		} else if (type == "IDAT") {
			compressed.insert(compressed.end(), data, data + length);
		} else if (type == "IEND") {
			break;
		}
		pos += 12 + length;
	}
	if (channels == 0) {
		throw std::runtime_error{ "png : no header" };
	}
	const auto raw = Inflate(compressed.data(), compressed.size());
	const auto stride = static_cast<std::size_t>(width) * static_cast<std::size_t>(channels);
	if (raw.size() < (stride + 1) * static_cast<std::size_t>(height)) {
		throw std::runtime_error{ "png : not enough data" };
	}
	auto pixels = std::vector<std::uint8_t>(stride * static_cast<std::size_t>(height));
	const auto bpp = static_cast<std::size_t>(channels);
	for (auto y = std::size_t{ 0 }; y < static_cast<std::size_t>(height); y++) {
		const auto filter = raw[y * (stride + 1)];
		const auto* line = &raw[y * (stride + 1) + 1];
		auto* out = &pixels[y * stride];
		const auto* prior = y > 0 ? &pixels[(y - 1) * stride] : nullptr;
		for (auto i = std::size_t{ 0 }; i < stride; i++) {
			const int a = i >= bpp ? out[i - bpp] : 0;
			const int b = prior ? prior[i] : 0;
			const int c = prior && i >= bpp ? prior[i - bpp] : 0;
			auto predictor = 0;
			switch (filter) {
			case 0: break;
			case 1: predictor = a; break;
			case 2: predictor = b; break;
			case 3: predictor = (a + b) / 2; break;
			case 4: {
				const auto p = a + b - c;
				const auto pa = std::abs(p - a), pb = std::abs(p - b), pc = std::abs(p - c);
				predictor = pa <= pb && pa <= pc ? a : pb <= pc ? b : c;
				break;
			}
			default: throw std::runtime_error{ "png : a bad filter" };
			}
			out[i] = static_cast<std::uint8_t>(line[i] + predictor);
		}
	}
	auto image = RgbImage{ width, height };
	for (auto i = std::size_t{ 0 }; i < static_cast<std::size_t>(width) * static_cast<std::size_t>(height); i++) {
		for (auto k = 0u; k < 3; k++) {
			image.rgb[i * 3 + k] = pixels[i * bpp + k];
		}
	}
	return image;
}

worldgen::RgbImage worldgen::ReadPng(const std::string& path) {
	auto in = std::ifstream{ path, std::ios::binary };
	if (!in) {
		throw std::runtime_error{ "png : cannot read " + path };
	}
	return DecodePng(std::vector<std::uint8_t>(std::istreambuf_iterator<char>{ in }, std::istreambuf_iterator<char>{}));
}

std::vector<std::uint8_t> worldgen::EncodeBmp(const RgbImage& image) {
	const auto stride = (static_cast<std::size_t>(image.width) * 3 + 3) / 4 * 4;
	const auto dataSize = stride * static_cast<std::size_t>(image.height);
	auto out = std::vector<std::uint8_t>(54 + dataSize, 0);
	const auto put32 = [&out](std::size_t at, std::uint32_t v) {
		for (auto k = 0u; k < 4; k++) {
			out[at + k] = static_cast<std::uint8_t>(v >> (8 * k));
		}
	};
	out[0] = 'B';
	out[1] = 'M';
	put32(2, static_cast<std::uint32_t>(out.size()));
	put32(10, 54);
	put32(14, 40);
	put32(18, static_cast<std::uint32_t>(image.width));
	put32(22, static_cast<std::uint32_t>(image.height));
	out[26] = 1;
	out[28] = 24;
	put32(34, static_cast<std::uint32_t>(dataSize));
	put32(38, 2835);
	put32(42, 2835);
	for (auto y = 0; y < image.height; y++) {
		auto* row = &out[54 + static_cast<std::size_t>(image.height - 1 - y) * stride];
		for (auto x = 0; x < image.width; x++) {
			const auto* p = image.at(x, y);
			row[x * 3] = p[2];
			row[x * 3 + 1] = p[1];
			row[x * 3 + 2] = p[0];
		}
	}
	return out;
}
