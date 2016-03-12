#pragma once
#include <string>
#include <SDL2\SDL_pixels.h>
#include "../Utils/IStringAble.h"

namespace ska {
	class Window;
	class TextureData : public IStringAble
	{
	public:
		TextureData(Window& window, std::string, SDL_Color);
		TextureData();

		void operator=(const TextureData& t2);

		std::pair<std::string, SDL_Color>& getData();
		Window& getWindow();

		virtual std::string toString() const;
		virtual ~TextureData();

	private:
		std::pair<std::string, SDL_Color> m_data;
		Window* m_window;
	};
}
