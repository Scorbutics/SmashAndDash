#include "WindowRenderer.h"

int WindowRenderer::render(ska::Window*, ska::Texture* tex, int x, int y, ska::Rectangle* clip) const{
	ska::Rectangle destBuf = { x, y, static_cast<int>(tex->getWidth()), static_cast<int>(tex->getHeight()) };

	if (clip != nullptr) {
		destBuf.w = clip->w;
		destBuf.h = clip->h;
	}


	//return SDL_RenderCopy(win->getRenderer(), tex->getInstance()->getInstancer(), clip, &destBuf);
	return 0;
}
