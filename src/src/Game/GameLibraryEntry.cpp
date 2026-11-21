// Entry points of the game library ("libGame"), loaded and called by the ska launcher (ska_main).
#include "Game/__internalConfig/LoggerConfig.h"
#include <memory>
#include "Core/GameApp.h"
#include "Core/Exceptions/GenericException.h"
#include "Core/Exceptions/TerminateProcessException.h"
#include "Graphic/SDLFont.h"
#include "GUI/GUI.h"

#if defined(_WIN32)
#define SKA_GAME_EXPORT extern "C" __declspec(dllexport)
#else
#define SKA_GAME_EXPORT extern "C" __attribute__((visibility("default")))
#endif

namespace {
	std::unique_ptr<ska::GameApp>& Game() {
		static std::unique_ptr<ska::GameApp> game;
		return game;
	}
}

SKA_GAME_EXPORT void init() {
	// The game resources are in the game directory, not in the ska "resources" one
	ska::SDLFont::DEFAULT_FONT_FILE = "Fonts/FiraSans-Medium.ttf";
	ska::GUI::MENU_DEFAULT_THEME_PATH = "Menu/default_theme/";
	Game() = ska::GameApp::instantiate();
}

SKA_GAME_EXPORT int run() {
	auto& game = Game();
	if (game == nullptr) {
		return -1;
	}
	auto result = 0;
	try {
		game->run();
	} catch (ska::TerminateProcessException& tpe) {
		result = game->onTerminate(tpe);
	} catch (ska::GenericException& e) {
		result = game->onException(e);
	}
	game = nullptr;
	return result;
}
