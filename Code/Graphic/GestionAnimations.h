#ifndef DEF_GANIMATION
#define DEF_GANIMATION

#include <vector>
#include <memory>
#include "Rectangle.h"

class Layer;

ska::Rectangle TileAnimationManagement(Layer *monde, ska::Rectangle posMap, ska::Rectangle& OfChip);
#endif
