#include "WindowRenderer.h"

int WindowRenderer::render(ska::Window* win, ska::Texture* tex, int x, int y, ska::Rectangle* clip)
{
	ska::Rectangle destBuf = { x, y, tex->getWidth(), tex->getHeight() };

	if (clip != NULL)
	{
		destBuf.w = clip->w;
		destBuf.h = clip->h;
	}


	//return SDL_RenderCopy(win->getRenderer(), tex->getInstance()->getInstancer(), clip, &destBuf);
	return 0;
}
