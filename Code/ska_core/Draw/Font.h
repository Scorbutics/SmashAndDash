#pragma once
#include "SDLFont.h"
#include "../Utils/ResourceTemplate.h"

namespace ska {
	class Font : public ResourceTemplate<SDLFont, int> {

	public:
		Font();
		explicit Font(unsigned int fontSize);
		TTF_Font* getInstance() const;
		virtual ~Font();

	};
}

