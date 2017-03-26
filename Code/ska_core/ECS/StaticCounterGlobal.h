#pragma once
namespace ska {
	class StaticCounterGlobal {
    protected:
        static unsigned int increment() {
            static bool initialized = false;
            if(!initialized) {
                initialized = true;
                ska::StaticCounterGlobal::m_componentTypeCounter = 0;
            }
            return ska::StaticCounterGlobal::m_componentTypeCounter++;
        }
	private:
		static unsigned int m_componentTypeCounter;
	};
}
