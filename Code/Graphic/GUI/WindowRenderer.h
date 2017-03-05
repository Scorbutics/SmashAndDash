#pragma once

#include "../../ska/Graphic/Rectangle.h"
#include "../../ska/Utils/Singleton_template.h"
#include "../../ska/Graphic/GUI/Window.h"
#include "../../ska/Graphic/Texture.h"

class WindowRenderer : public ska::Singleton<WindowRenderer> {
	friend class ska::Singleton<WindowRenderer>;

private:
	WindowRenderer() {}
	~WindowRenderer() {}

public:		
	int render(ska::Window* win, ska::Texture* tex, int x, int y, ska::Rectangle* clip = nullptr) const;
		
};
