#include <iostream>
#include "Gameplay/WGameCore.h"
#include "Exceptions/GenericException.h"
#include "Exceptions/TerminateProcessException.h"
#include "MessagePopup.h"
#include "./Utils/IDs.h"

#define SKA_DEBUG

int main( int argc , char ** argv ) {
	(void) argc;
    (void) argv;

	ska::IniReader reader("gamesettings.ini");
	auto widthBlocks = reader.get<int>("Window width_blocks");
	auto heightBlocks = reader.get<int>("Window height_blocks");
	const auto& title = reader.get<std::string>("Window title");

	try {
		WGameCore app(title, widthBlocks * TAILLEBLOC, heightBlocks * TAILLEBLOC);
		while (app.run());
	} catch (ska::TerminateProcessException& tpe) {
		std::clog << tpe.what() << std::endl;
	} catch (ska::GenericException& e) {
		/* Handles Generics Game exceptions */
		std::cerr << e.what() << std::endl;
		ska::MessagePopup(ska::MessageType::Enum::Error, "Uncaught exception occured", e.what(), nullptr);
	}

    return 0;

}









