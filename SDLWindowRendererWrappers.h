#include <SDL2/SDL.h>

struct smart_sdl_window
{
	SDL_Window *handle;

	explicit smart_sdl_window(char const *name, int x, int y, int w, int h, Uint32 flags) :
		handle(SDL_CreateWindow(name, x, y, w, h, flags))
	{}

	~smart_sdl_window()
	{
		SDL_DestroyWindow(handle);
	}
};

struct smart_sdl_renderer
{
	SDL_Renderer *handle;

	explicit smart_sdl_renderer(SDL_Window* win, int index, Uint32 flags) :
		handle(SDL_CreateRenderer(win, index, flags))
	{}

	~smart_sdl_renderer()
	{
		SDL_DestroyRenderer(handle);
	}
};

