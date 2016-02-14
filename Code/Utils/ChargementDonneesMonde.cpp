#include <iostream>
#include <windows.h>
#include <fstream>
#include <sstream>
#include "../Inputs\Readers\IniReader.h"

using namespace std;

void LoadGameCoreData(int& widthBlocks, int& heightBlocks)
{
	IniReader reader("gamesettings.ini");


	widthBlocks = reader.getInt("Settings window_width_blocks");
	heightBlocks = reader.getInt("Settings window_height_blocks");


}


