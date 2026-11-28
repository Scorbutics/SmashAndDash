#pragma once
#include <functional>
#include <map>
#include <string>
#include "Base/Patterns/SubObserver.h"
#include "Core/Data/Events/DebugGUIEvent.h"
#include "Core/Data/Events/InputKeyEvent.h"
#include "Canvas.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"

class WorldState;

namespace ska {
	class DrawableContainer;
}

// The panel of the developers (F3 shows it, hides it ; SAD_DEBUG=1 : shown at the start), drawn as the menus : the lines
// given by the DebugGUIEvent (DebugDrawer : the frames per second, the physics, the position of the trainer, the nearest
// wild Pokemon), each read again at its own pace, and the map
class DebugPanel :
	public ska::SubObserver<ska::DebugGUIEvent>,
	public ska::SubObserver<ska::InputKeyEvent> {
public:
	DebugPanel(PokemonGameEventDispatcher& ged, WorldState& worldState);
	DebugPanel(const DebugPanel&) = delete;
	DebugPanel& operator=(const DebugPanel&) = delete;
	~DebugPanel() override = default;

	void refresh(unsigned int ellapsedTime);
	void graphicUpdate(ska::DrawableContainer& drawables);

private:
	struct Line {
		std::string label;
		std::function<std::string()> read;
		unsigned int delay = 1000;
		unsigned int age = 0;
		std::string value;
	};

	bool onDebug(ska::DebugGUIEvent& event);
	bool onKey(ska::InputKeyEvent& event);
	void render(Ui::Canvas& canvas) const;

	WorldState& m_worldState;
	Ui::Layer m_layer;
	// In the order they were given
	std::map<std::string, Line> m_lines;
	std::vector<std::string> m_order;
	bool m_shown;
	bool m_lastKey = true;
};
