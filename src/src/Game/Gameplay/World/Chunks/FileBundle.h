#pragma once
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

// Files as one block of bytes (plain C++, tests/chunks) : the messages of the web worker of the world (world_worker.js :
// the data of the generator sent to it, the files of a chunk it wrote sent back).
namespace chunks {
	// Their paths (relative, "/" between the folders) and their bytes
	using FileBundle = std::vector<std::pair<std::string, std::vector<std::uint8_t>>>;

	std::vector<std::uint8_t> PackBundle(const FileBundle& files);
	// Throws std::runtime_error on bytes that are not a bundle
	FileBundle UnpackBundle(const std::uint8_t* data, std::size_t size);

	// The files under a folder (recursively ; their paths from it), the files of a bundle written under a folder (the
	// folders made, the files there replaced)
	FileBundle ReadBundle(const std::string& folder);
	void WriteBundle(const FileBundle& files, const std::string& folder);
}
