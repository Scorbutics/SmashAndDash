#include "GraphicComponent.h"

ska::GraphicComponent::GraphicComponent() {
    desiredPriority = std::numeric_limits<int>().min();
}

void ska::GraphicComponent::operator=(const GraphicComponent& gc) {
    sprite = gc.sprite;
    desiredPriority = gc.desiredPriority;
}

