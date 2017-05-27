#pragma once
#include "Exceptions/TerminateProcessException.h"
#include "Ticked.h"

namespace ska {
	class GenericException;

	class GameApp : public Ticked {
	protected:
		GameApp();

	public:
		static GameApp& get();
		
		virtual int onTerminate(TerminateProcessException&) { return 0; };
		virtual int onException(GenericException&) = 0;

		virtual void run() = 0;

		virtual float ticksWanted() const override {
			const static auto FPS = 60U;
			const static float TICKS = 1000.F / FPS;
			return TICKS;
		}

		virtual ~GameApp();
	};
}
