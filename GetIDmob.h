#ifndef DEF_GETIDMOB
#define DEF_GETIDMOB

#include <vector>
#include "Character.h"

#include <memory>
typedef std::unique_ptr<Character> Character_ptr;

using namespace std;

SDL_Rect GetIDmob();

#endif
