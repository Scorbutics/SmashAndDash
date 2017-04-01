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

	auto widthBlocks = 0, heightBlocks = 0;
    std::string startMapName, startMapChipsetName;

	ska::IniReader reader("gamesettings.ini");
	widthBlocks = reader.get<int>("Window width_blocks");
	heightBlocks = reader.get<int>("Window height_blocks");
	const auto& title = reader.get<std::string>("Window title");

	try {
		//(widthBlocks*TAILLEBLOC > TAILLEECRANMINX ? widthBlocks*TAILLEBLOC: TAILLEECRANMINX), (heightBlocks*TAILLEBLOC > TAILLEECRANMINY ? heightBlocks*TAILLEBLOC: TAILLEECRANMINY)

		//Crée une fenetre de type "WGameCore", Génère ce monde sur la fenetre (unique)
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









