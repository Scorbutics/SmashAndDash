#include <sstream>
#include "Font.h"


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
