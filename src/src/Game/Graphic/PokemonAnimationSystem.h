#pragma once
#include "Graphic/System/AnimationSystem.h"
#include "Graphic/System/WalkAnimationStateMachine.h"
#include "Graphic/System/JumpAnimationStateMachine.h"
#include "Graphic/System/RunAnimationStateMachine.h"
#include "Graphic/System/PoseAnimationStateMachine.h"

using PokemonAnimationSystem = ska::AnimationSystem<ska::WalkAnimationStateMachine, ska::JumpAnimationStateMachine, ska::RunAnimationStateMachine, ska::PoseAnimationStateMachine>;
