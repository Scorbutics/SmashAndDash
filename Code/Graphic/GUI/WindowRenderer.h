#ifndef DEF_WINDOWRENDERER
#define DEF_WINDOWRENDERER

#include <memory>
#include "../Rectangle.h"
#include "../../Utils/Singleton_template.h"

class Texture;
class Window;
class WindowRenderer : public Singleton<WindowRenderer>
{
	friend class Singleton<WindowRenderer>;

	private:
		WindowRenderer() {}
		~WindowRenderer() {}

	public:		
		int render(Window* win, Texture* tex, int x, int y, ska::Rectangle* clip = NULL);
		
};

#endif
