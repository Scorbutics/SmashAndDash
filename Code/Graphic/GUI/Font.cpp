#include <sstream>
#include "..\Font.h"
#include "../../Utils\ResourceTemplate.h"

using namespace std;

Font::Font() : ResourceTemplate()
{
}

Font::Font(unsigned int fontSize) : ResourceTemplate(UInteger(fontSize))
{
}

TTF_Font* Font::getInstance()
{
	return m_value->getInstance();
}

Font::~Font()
{
	
}
