#include <SDL_timer.h>
#include "TimeUtils.h"

ska::TimeUtils::TimeUtils() {

}

unsigned int ska::TimeUtils::getTicks() {
	return SDL_GetTicks();
}

ska::TimeUtils::~TimeUtils() {

}
