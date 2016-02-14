#ifndef DEF_FONT
#define DEF_FONT


#include "SDLFont.h"
#include "Utils\ResourceTemplate.h"

class Font : public ResourceTemplate<SDLFont, UInteger>
{

	public:
		Font();
		Font(unsigned int fontSize);

		TTF_Font* getInstance();

		virtual ~Font();
		
		
};

#endif

