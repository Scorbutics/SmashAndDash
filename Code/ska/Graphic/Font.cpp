#include <sstream>
#include "Font.h"
#include "../Utils/ResourceTemplate.h"

using namespace std;

ska::Font::Font() : ResourceTemplate() {
}

ska::Font::Font(unsigned int fontSize) : ResourceTemplate(fontSize) {
}

TTF_Font* ska::Font::getInstance() const {
	return m_value->getInstance();
}

ska::Font::~Font()
{
	
}
