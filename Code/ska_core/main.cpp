#include <SDL.h>
//We need this include to define "main" as "SDL_main"

#include "GameApp.h"
#include "Utils/OutputUtils.h"

namespace ska {
	class TerminateProcessException;
}

int main(int argc, char * argv[]) {
	(void)argc;
	(void)argv;
	
	auto& gameApp = ska::GameApp::get();

	try {
		gameApp.run();
	} catch (ska::TerminateProcessException& tpe) {
		return gameApp.onTerminate(tpe);
	} catch (ska::GenericException& e) {
		return gameApp.onException(e);
	}

	return 0;
}
