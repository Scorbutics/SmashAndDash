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

// The save chosen on the title screen (StateTitle), else empty
inline std::string& ChosenSaveRef() {
	static auto name = std::string{};
	return name;
}

// The save given by the environment (SAD_SAVE : the agent runs, ?SAD_SAVE= on the web), else empty : the title screen
inline std::string EnvironmentSaveName() {
	const auto* name = std::getenv("SAD_SAVE");
	return name == nullptr ? std::string{} : std::string{ name };
}

// The save the game plays : the one chosen on the title screen, else SAD_SAVE, else "save1"
inline std::string DefaultSaveName() {
	if (!ChosenSaveRef().empty()) {
		return ChosenSaveRef();
	}
	const auto name = EnvironmentSaveName();
	return name.empty() ? std::string{ "save1" } : name;
}

// The folder of the maps of the world of the save ([World] directory of its trainer.ini, ex : ./Data/Worlds/seed7/ :
// tools/worldgen), else ./Levels/. Set when the save is loaded (SavegameManager::loadTrainer).
inline std::string& WorldDirectoryRef() {
	static auto directory = std::string{ "./Levels/" };
	return directory;
}

inline const std::string& WorldDirectory() {
	return WorldDirectoryRef();
}

// The folder of a map of the world (its layers, its .ini, its events, its wild Pokemon, its light)
inline std::string MapDirectory(const std::string& map) {
	return WorldDirectory() + map;
}

// The .ini of a map
inline std::string MapIni(const std::string& map) {
	return MapDirectory(map) + "/" + map + ".ini";
}

// A tileset name from the "Chipset file" of a map (the image without its extension)
inline std::string TilesetName(const std::string& chipsetFile) {
	const auto dot = chipsetFile.rfind('.');
	const auto slash = chipsetFile.rfind('/');
	return dot == std::string::npos || (slash != std::string::npos && dot < slash) ? chipsetFile : chipsetFile.substr(0, dot);
}
