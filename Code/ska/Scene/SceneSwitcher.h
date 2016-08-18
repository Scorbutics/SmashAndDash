#pragma once
#include <iostream>

namespace ska {
	class Scene;
	class SceneHolder;
	class Window;
	class InputContextManager;


	template <typename T>
	class SceneSwitcher {
	public:
		SceneSwitcher(const std::string& map, const std::string& chipsetName) :
			m_mapName(map),
			m_chipsetName(chipsetName) {}
		
		virtual ~SceneSwitcher() = default;

		virtual void switchTo(ska::Window& w, ska::SceneHolder& holder, ska::Scene& lastScene, ska::InputContextManager& icm, T extraArg) const = 0;

	protected:
		const std::string m_mapName;
		const std::string m_chipsetName;
	};
}