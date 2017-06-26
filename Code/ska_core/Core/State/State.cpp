#include "StateHolder.h"
#include "State.h"

ska::State::State(StateHolder& holder) : m_holder(holder) {
}

ska::State::State(State& oldScene) :
  m_holder(oldScene.m_holder)  {
}
