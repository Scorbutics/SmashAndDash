#pragma once


#include <string>
#include <SDL2/SDL.h>
#include "Utils/IStringAble.h"

class TextureData : public IStringAble
{
	public:
		TextureData(std::string, SDL_Color);
		TextureData();

		std::pair<std::string, SDL_Color>& getData();

		virtual std::string toString() const;
		virtual ~TextureData();

	private:
		std::pair<std::string, SDL_Color> m_data;

};

