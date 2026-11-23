#pragma once
#include <cstdlib>
#include <string>

// Where the saves are : SAD_SAVE_DIR (environment), else ./Data/Saves. An agent run (tools/sad-run) gives a copy of a
// save fixture in its run folder : the saves of the repository are never written by it.
inline std::string SaveDirectory() {
	const auto* directory = std::getenv("SAD_SAVE_DIR");
	if (directory == nullptr || *directory == '\0') {
		return "./Data/Saves/";
	}
	auto path = std::string{ directory };
	return path.back() == '/' ? path : path + "/";
}

// The save loaded at the start : SAD_SAVE (environment), else "save1"
inline std::string DefaultSaveName() {
	const auto* name = std::getenv("SAD_SAVE");
	return name == nullptr || *name == '\0' ? std::string{ "save1" } : std::string{ name };
}
