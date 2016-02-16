#include <iostream>
#include "CommandElsif.h"


using namespace std;

CommandElsif::CommandElsif()
{
}

const string& CommandElsif::getCmdName() {
	return ControlStatement::getCommandElsif();
}

CommandElsif::~CommandElsif()
{
}
