#pragma once
#include "Exceptions/TerminateProcessException.h"

namespace ska {
	class GenericException;

	class GameApp {
	protected:
		GameApp();

	public:
		static GameApp& get();
		
		virtual int onTerminate(TerminateProcessException&) { return 0; };
		virtual int onException(GenericException&) = 0;

		virtual void run() = 0;

		virtual unsigned int ticksWanted() const {
			const unsigned int FPS = 60;
			const unsigned int TICKS = 1000 / FPS;
			return TICKS;
		}

		virtual ~GameApp();
	};
}
