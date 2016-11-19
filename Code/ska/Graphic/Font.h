#ifndef DEF_FONT
#define DEF_FONT


#include "SDLFont.h"
#include "../Utils/ResourceTemplate.h"

namespace ska {
	class Font : public ResourceTemplate<SDLFont, int>
	{

	public:
		Font();
		Font(unsigned int fontSize);

		TTF_Font* getInstance() const;

		virtual ~Font();


	};
}
#endif

