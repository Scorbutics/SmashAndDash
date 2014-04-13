#include "WindowRenderer.h"
#include "Texture.h"
#include "Window.h"

int WindowRenderer::render(Window* win, Texture* tex, int x, int y, SDL_Rect* clip)
{
	SDL_Rect destBuf = { x, y, tex->getWidth(), tex->getHeight() };

	if (clip != NULL)
	{
		destBuf.w = clip->w;
		destBuf.h = clip->h;
	}


	//return SDL_RenderCopy(win->getRenderer(), tex->getInstance()->getInstancer(), clip, &destBuf);
	return 0;
}
