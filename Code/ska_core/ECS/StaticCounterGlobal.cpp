#include "StaticCounterGlobal.h"

unsigned int ska::StaticCounterGlobal::increment() {
    static unsigned int componentTypeCounter = 0;
    return componentTypeCounter++;
}
