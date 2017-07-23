#pragma once

namespace ska {
	enum GameEventType {
		GAME_WINDOW_READY,
		GAME_WINDOW_RESIZED
	};

	class GameEvent {
	public:
		explicit GameEvent(GameEventType get) : 
			m_eventType(get),
			windowWidth(0),
			windowHeight(0){
		}

		virtual ~GameEvent() = default;
		const GameEventType& getEventType() const{
			return m_eventType;
		}

		
		unsigned int windowWidth;
		unsigned int windowHeight;
	private:
		const GameEventType m_eventType;

	};
}
