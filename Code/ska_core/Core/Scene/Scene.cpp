#include "SceneHolder.h"
#include "Scene.h"

ska::Scene::Scene(SceneHolder& sh) :
m_holder(sh) {
}

ska::Scene::Scene(Scene& oldScene):
m_holder(oldScene.m_holder) {

}
