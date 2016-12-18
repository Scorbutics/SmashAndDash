#include "../../ska/Graphic/GUI/GUI.h"

class Settings;

class GUIMap : public ska::GUI {
public:
	GUIMap(ska::Window& w, ska::InputContextManager& playerICM);
	
	void bind(Settings& sets);

	~GUIMap() = default;
	
};