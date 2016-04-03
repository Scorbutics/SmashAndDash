#pragma once
#include "../../ska/ECS/IGraphicSystem.h"
#include "../../ska/Graphic/PositionnedGraphicDrawable.h"

class CameraSystem;

class AbstractGraphicSystem : public ska::IGraphicSystem
{
public:
	AbstractGraphicSystem(ska::CameraSystem* camera);
	virtual ~AbstractGraphicSystem();
	void linkCamera(ska::CameraSystem* camera) override;
	void setDrawables(ska::DrawableContainer& container) override;

protected:
	ska::CameraSystem* m_camera;
	ska::DrawableContainer* m_drawables;
	std::vector<ska::PositionnedGraphicDrawable> m_pgd;
};
