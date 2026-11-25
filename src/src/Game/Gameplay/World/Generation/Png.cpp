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

namespace {
	class BitWriter {
	public:
		explicit BitWriter(std::vector<std::uint8_t>& out) : m_out(out) {}
		void put(std::uint32_t bits, int count) {
			m_buffer |= bits << m_count;
			m_count += count;
			while (m_count >= 8) {
				m_out.push_back(static_cast<std::uint8_t>(m_buffer & 0xFF));
				m_buffer >>= 8;
				m_count -= 8;
			}
		}
		// A Huffman code : its bits from the most significant one
		void code(std::uint32_t bits, int count) {
			auto reversed = 0u;
			for (auto i = 0; i < count; i++) {
				reversed = (reversed << 1) | ((bits >> i) & 1);
			}
			put(reversed, count);
		}
		void flush() {
			if (m_count > 0) {
				m_out.push_back(static_cast<std::uint8_t>(m_buffer & 0xFF));
			}
			m_buffer = 0;
			m_count = 0;
		}
	private:
		std::vector<std::uint8_t>& m_out;
		std::uint32_t m_buffer = 0;
		int m_count = 0;
	};

	void Literal(BitWriter& out, int symbol) {
		if (symbol < 144) {
			out.code(static_cast<std::uint32_t>(0x30 + symbol), 8);
		} else if (symbol < 256) {
			out.code(static_cast<std::uint32_t>(0x190 + symbol - 144), 9);
		} else if (symbol < 280) {
			out.code(static_cast<std::uint32_t>(symbol - 256), 7);
		} else {
			out.code(static_cast<std::uint32_t>(0xC0 + symbol - 280), 8);
		}
	}

	void Match(BitWriter& out, int length, int distance) {
		auto l = LengthBase.size() - 1;
		while (LengthBase[l] > length) {
			l--;
		}
		Literal(out, 257 + static_cast<int>(l));
		out.put(static_cast<std::uint32_t>(length - LengthBase[l]), LengthExtra[l]);
		auto d = DistBase.size() - 1;
		while (DistBase[d] > distance) {
			d--;
		}
		out.code(static_cast<std::uint32_t>(d), 5);
		out.put(static_cast<std::uint32_t>(distance - DistBase[d]), DistExtra[d]);
	}

	std::uint32_t Crc32(const std::uint8_t* data, std::size_t size, std::uint32_t crc = 0) {
		static const auto table = [] {
			auto t = std::array<std::uint32_t, 256>{};
			for (auto n = 0u; n < 256; n++) {
				auto c = n;
				for (auto k = 0; k < 8; k++) {
					c = c & 1 ? 0xEDB88320u ^ (c >> 1) : c >> 1;
				}
				t[n] = c;
			}
			return t;
		}();
		crc = ~crc;
		for (auto i = std::size_t{ 0 }; i < size; i++) {
			crc = table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
		}
		return ~crc;
	}

	void PutBigEndian(std::vector<std::uint8_t>& out, std::uint32_t v) {
		for (auto k = 3; k >= 0; k--) {
			out.push_back(static_cast<std::uint8_t>(v >> (8 * k)));
		}
	}

	void Chunk(std::vector<std::uint8_t>& out, const char* type, const std::vector<std::uint8_t>& data) {
		PutBigEndian(out, static_cast<std::uint32_t>(data.size()));
		const auto start = out.size();
		out.insert(out.end(), type, type + 4);
		out.insert(out.end(), data.begin(), data.end());
		PutBigEndian(out, Crc32(&out[start], out.size() - start));
	}
}

std::vector<std::uint8_t> worldgen::Deflate(const std::vector<std::uint8_t>& data) {
	constexpr int Window = 32768;
	constexpr int MinMatch = 3;
	constexpr int MaxMatch = 258;
	constexpr int MaxChain = 24;
	constexpr int HashBits = 15;
	auto out = std::vector<std::uint8_t>{ 0x78, 0x01 };
	auto bits = BitWriter{ out };
	bits.put(1, 1); // the last block
	bits.put(1, 2); // the fixed codes
	auto head = std::vector<int>(1 << HashBits, -1);
	auto prev = std::vector<int>(Window, -1);
	const auto n = static_cast<int>(data.size());
	const auto hash = [&data](int i) {
		return ((static_cast<unsigned>(data[static_cast<std::size_t>(i)]) << 10) ^ (static_cast<unsigned>(data[static_cast<std::size_t>(i + 1)]) << 5)
			^ data[static_cast<std::size_t>(i + 2)]) & ((1u << HashBits) - 1);
	};
	const auto insert = [&](int i) {
		if (i + MinMatch <= n) {
			const auto h = hash(i);
			prev[static_cast<std::size_t>(i % Window)] = head[h];
			head[h] = i;
		}
	};
	auto pos = 0;
	while (pos < n) {
		auto bestLength = 0;
		auto bestDistance = 0;
		if (pos + MinMatch <= n) {
			auto candidate = head[hash(pos)];
			const auto limit = std::min(MaxMatch, n - pos);
			for (auto chain = 0; chain < MaxChain && candidate >= 0 && pos - candidate <= Window; chain++) {
				auto length = 0;
				while (length < limit && data[static_cast<std::size_t>(candidate + length)] == data[static_cast<std::size_t>(pos + length)]) {
					length++;
				}
				if (length > bestLength) {
					bestLength = length;
					bestDistance = pos - candidate;
					if (length == limit) {
						break;
					}
				}
				const auto next = prev[static_cast<std::size_t>(candidate % Window)];
				if (next >= candidate) {
					break;
				}
				candidate = next;
			}
		}
		if (bestLength >= MinMatch) {
			Match(bits, bestLength, bestDistance);
			// A long match (the runs of zeros of the filtered rows) : only its end is indexed, as zlib does
			for (auto i = bestLength > 32 ? bestLength - 4 : 0; i < bestLength; i++) {
				insert(pos + i);
			}
			pos += bestLength;
		} else {
			Literal(bits, data[static_cast<std::size_t>(pos)]);
			insert(pos);
			pos++;
		}
	}
	Literal(bits, 256);
	bits.flush();
	auto a = 1u, b = 0u;
	for (const auto byte : data) {
		a = (a + byte) % 65521;
		b = (b + a) % 65521;
	}
	PutBigEndian(out, (b << 16) | a);
	return out;
}

std::vector<std::uint8_t> worldgen::EncodePng(const RgbImage& image, bool alpha) {
	const auto channels = alpha ? 4u : 3u;
	const auto stride = static_cast<std::size_t>(image.width) * channels;
	auto raw = std::vector<std::uint8_t>{};
	raw.reserve((stride + 1) * static_cast<std::size_t>(image.height));
	auto line = std::vector<std::uint8_t>(stride);
	auto previous = std::vector<std::uint8_t>(stride, 0);
	auto filtered = std::array<std::vector<std::uint8_t>, 5>{};
	for (auto& f : filtered) {
		f.resize(stride);
	}
	for (auto y = 0; y < image.height; y++) {
		for (auto x = 0; x < image.width; x++) {
			const auto* p = image.at(x, y);
			for (auto k = 0u; k < 3; k++) {
				line[static_cast<std::size_t>(x) * channels + k] = p[k];
			}
			if (alpha) {
				line[static_cast<std::size_t>(x) * channels + 3] = 255;
			}
		}
		// The filter of the row : up (zeros) for a row like the one above (a tile is drawn by voxels of 3 x 3 pixels),
		// else sub (zeros along a voxel), else paeth, the one with the smallest sum of its bytes (as signed values)
		auto best = 2u;
		if (y == 0 || line != previous) {
			auto bestSum = ~std::uint64_t{ 0 };
			for (const auto f : { 1u, 4u }) {
				auto sum = std::uint64_t{ 0 };
				for (auto i = std::size_t{ 0 }; i < stride; i++) {
					const int a = i >= channels ? line[i - channels] : 0;
					auto predictor = a;
					if (f == 4) {
						const int b = previous[i];
						const int c = i >= channels ? previous[i - channels] : 0;
						const auto p = a + b - c;
						const auto pa = std::abs(p - a), pb = std::abs(p - b), pc = std::abs(p - c);
						predictor = pa <= pb && pa <= pc ? a : pb <= pc ? b : c;
					}
					const auto v = static_cast<std::uint8_t>(line[i] - predictor);
					filtered[f][i] = v;
					sum += static_cast<std::uint64_t>(v < 128 ? v : 256 - v);
				}
				if (sum < bestSum) {
					bestSum = sum;
					best = f;
				}
			}
		} else {
			std::fill(filtered[2].begin(), filtered[2].end(), std::uint8_t{ 0 });
		}
		raw.push_back(static_cast<std::uint8_t>(best));
		raw.insert(raw.end(), filtered[best].begin(), filtered[best].end());
		std::swap(line, previous);
	}
	auto out = std::vector<std::uint8_t>{ 137, 80, 78, 71, 13, 10, 26, 10 };
	auto header = std::vector<std::uint8_t>{};
	PutBigEndian(header, static_cast<std::uint32_t>(image.width));
	PutBigEndian(header, static_cast<std::uint32_t>(image.height));
	header.insert(header.end(), { 8, static_cast<std::uint8_t>(alpha ? 6 : 2), 0, 0, 0 });
	Chunk(out, "IHDR", header);
	Chunk(out, "IDAT", Deflate(raw));
	Chunk(out, "IEND", {});
	return out;
}

void worldgen::WriteFile(const std::string& path, const std::vector<std::uint8_t>& bytes) {
	auto file = std::ofstream{ path, std::ios::binary | std::ios::trunc };
	if (!file) {
		throw std::runtime_error{ "worldgen : cannot write " + path };
	}
	file.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
}
