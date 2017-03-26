#pragma once

namespace ska {

	class Music;
	enum WorldEventType {
		WORLD_CREATE,
		WORLD_CHANGE,
		WORLD_QUIT
	};

	class WorldEvent {
	public:
		WorldEvent(WorldEventType wet) : m_type(wet) {
		}

		void setBgm(Music& m) {
			m_bgm = &m;
		}

		Music* getBgm() const {
			return m_bgm;
		}

		const WorldEventType& getEventType() const {
			return m_type;
		}

		virtual ~WorldEvent() = default;

	private:
		WorldEventType m_type;
		Music* m_bgm;
	};

}
