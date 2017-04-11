#pragma once

namespace ska {
	enum GameLibrariesInitializedEventType {
		ALL
	};

	class GameLibrariesInitializedEvent {
	public:
		GameLibrariesInitializedEvent(GameLibrariesInitializedEventType glit) : m_eventType(glit) {
		}
		virtual ~GameLibrariesInitializedEvent() = default;

		const GameLibrariesInitializedEventType& getEventType() const {
            return m_eventType;
		}

	private:
		const GameLibrariesInitializedEventType m_eventType;

	};
}
