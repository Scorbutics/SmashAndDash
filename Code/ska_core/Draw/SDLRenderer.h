#pragma once
#include <SDL.h>
#include "../Rectangle.h"

namespace ska {
    class SDLSurface;
    class SDLTexture;

	class SDLRenderer {
	public:
	    SDLRenderer();
		SDLRenderer(SDL_Window * window, int index, Uint32 flags);
		SDLRenderer(SDLRenderer&& r);

		SDLRenderer& operator=(SDLRenderer&& r);
		SDLRenderer(const SDLRenderer& r) = delete;
        SDLRenderer& operator=(const SDLRenderer& r) = delete;
        static void setDefaultRenderer(SDLRenderer* renderer);
        static SDLRenderer* getDefaultRenderer();

        void renderClear() const;

        SDL_Texture* createTextureFromSurface(const SDLSurface& s) const;
        void renderPresent() const;
        int renderCopy(const SDLTexture& t, const Rectangle* clip, const Rectangle& dest) const;

		~SDLRenderer();

		void load(SDL_Window * window, int index, Uint32 flags);

	private:
	    void free();
		SDL_Renderer* m_renderer;
		static SDLRenderer* m_currentDefaultRenderer;

	};
}
