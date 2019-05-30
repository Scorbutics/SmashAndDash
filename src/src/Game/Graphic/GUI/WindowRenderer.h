#pragma once

#include "Core/Rectangle.h"
#include "Base/Patterns/Singleton.h"
#include "Graphic/Texture.h"

namespace ska {
    class SDLRenderer;
}

class WindowRenderer : public ska::Singleton<WindowRenderer> {
	friend class ska::Singleton<WindowRenderer>;

private:
	WindowRenderer() {}
	~WindowRenderer() {}

public:
	int render(ska::SDLRenderer* win, ska::Texture* tex, int x, int y, ska::Rectangle* clip = nullptr) const;

};
