#ifndef DEF_GANIMATION
#define DEF_GANIMATION

#include <vector>
#include <memory>
#include <SDL2/SDL.h>

class Layer;

SDL_Rect TileAnimationManagement(Layer *monde, SDL_Rect posMap, SDL_Rect& OfChip);
#endif
