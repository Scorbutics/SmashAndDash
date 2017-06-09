#include "SceneHolder.h"
#include "Scene.h"

ska::Scene::Scene(SceneHolder& holder) : m_holder(holder) {
}

ska::Scene::Scene(Scene& oldScene) :
  m_holder(oldScene.m_holder)  {
}
