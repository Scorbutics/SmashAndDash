#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include "FileBundle.h"

namespace fs = std::filesystem;

namespace {
	void Put32(std::vector<std::uint8_t>& out, std::uint32_t value) {
		for (auto k = 0; k < 4; k++) {
			out.push_back(static_cast<std::uint8_t>(value >> (8 * k)));
		}
	}

	std::uint32_t Get32(const std::uint8_t* data, std::size_t size, std::size_t& at) {
		if (at + 4 > size) {
			throw std::runtime_error{ "file bundle : cut short" };
		}
		auto value = std::uint32_t{ 0 };
		for (auto k = 0; k < 4; k++) {
			value |= static_cast<std::uint32_t>(data[at + static_cast<std::size_t>(k)]) << (8 * k);
		}
		at += 4;
		return value;
	}
}

std::vector<std::uint8_t> chunks::PackBundle(const FileBundle& files) {
	auto out = std::vector<std::uint8_t>{};
	Put32(out, static_cast<std::uint32_t>(files.size()));
	for (const auto& [path, bytes] : files) {
		Put32(out, static_cast<std::uint32_t>(path.size()));
		out.insert(out.end(), path.begin(), path.end());
		Put32(out, static_cast<std::uint32_t>(bytes.size()));
		out.insert(out.end(), bytes.begin(), bytes.end());
	}
	return out;
}

chunks::FileBundle chunks::UnpackBundle(const std::uint8_t* data, std::size_t size) {
	auto at = std::size_t{ 0 };
	const auto count = Get32(data, size, at);
	auto files = FileBundle{};
	for (auto i = 0u; i < count; i++) {
		const auto pathSize = Get32(data, size, at);
		if (at + pathSize > size) {
			throw std::runtime_error{ "file bundle : cut short" };
		}
		auto path = std::string(reinterpret_cast<const char*>(data + at), pathSize);
		at += pathSize;
		const auto bytesSize = Get32(data, size, at);
		if (at + bytesSize > size) {
			throw std::runtime_error{ "file bundle : cut short" };
		}
		files.emplace_back(std::move(path), std::vector<std::uint8_t>(data + at, data + at + bytesSize));
		at += bytesSize;
	}
	return files;
}

chunks::FileBundle chunks::ReadBundle(const std::string& folder) {
	auto files = FileBundle{};
	if (!fs::exists(folder)) {
		return files;
	}
	for (const auto& entry : fs::recursive_directory_iterator{ folder }) {
		if (!entry.is_regular_file()) {
			continue;
		}
		auto in = std::ifstream{ entry.path(), std::ios::binary };
		files.emplace_back(fs::relative(entry.path(), folder).generic_string(),
			std::vector<std::uint8_t>(std::istreambuf_iterator<char>{ in }, std::istreambuf_iterator<char>{}));
	}
	return files;
}

void chunks::WriteBundle(const FileBundle& files, const std::string& folder) {
	for (const auto& [path, bytes] : files) {
		const auto file = fs::path{ folder } / path;
		fs::create_directories(file.parent_path());
		auto out = std::ofstream{ file, std::ios::binary | std::ios::trunc };
		if (!out) {
			throw std::runtime_error{ "file bundle : cannot write " + file.string() };
		}
		out.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
	}
}
