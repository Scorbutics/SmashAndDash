#pragma once
#include <string>

namespace ska {
	class Savegame
	{
	public:
		Savegame() {}

		virtual std::string getSaveName() const = 0;
		virtual int getGameVariable(const unsigned int x) const = 0;
		virtual bool getGameSwitch(const unsigned int x) const = 0;

		virtual void setGameVariable(const unsigned int x, const int value) = 0;
		virtual void setGameSwitch(const unsigned int x, const bool value) = 0;

		virtual ~Savegame() {}
	};
}
