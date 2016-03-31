#include "../Inputs/InputContextManager.h"
#include "../Graphic/Draw/DrawableContainer.h"
#include "../ECS/IGraphicSystem.h"
#include "../ECS/ISystem.h"
#include "SceneHolder.h"
#include "Scene.h"

ska::Scene::Scene(SceneHolder& sh, InputContextManager& ril) : 
m_inputCManager(ril), 
m_holder(sh) {
}

void ska::Scene::graphicUpdate(ska::DrawableContainer& drawables) {
	/* Graphics */
	for (ska::IGraphicSystem* s : m_graphics) {
		s->setDrawables(drawables);
		s->refresh();
	}
}

void ska::Scene::eventUpdate(bool movingDisallowed) {
	/* Raw input acquisition */
	m_inputCManager.refresh();

	/* Logics */
	for (ska::ISystem* s : m_logics) {
		s->refresh();
	}
}