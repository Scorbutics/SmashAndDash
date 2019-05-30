#pragma once
#include "Graphic/System/AnimationSystem.h"
#include "Graphic/System/WalkAnimationStateMachine.h"
#include "Graphic/System/JumpAnimationStateMachine.h"

using PokemonAnimationSystem = ska::AnimationSystem<ska::WalkAnimationStateMachine, ska::JumpAnimationStateMachine>;