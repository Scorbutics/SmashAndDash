#include <SDL_timer.h>
#include "TimeUtils.h"

ska::TimeUtils::TimeUtils() {

}

unsigned int ska::TimeUtils::getTicks() {
	return SDL_GetTicks();
}

void ska::TimeUtils::wait(unsigned int ms) {
    SDL_Delay(static_cast<Uint32>(ms));
}

ska::TimeUtils::~TimeUtils() {

}
