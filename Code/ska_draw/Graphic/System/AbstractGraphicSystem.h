#pragma once
#include "Draw/IGraphicSystem.h"
#include "../PositionnedGraphicDrawable.h"

namespace ska {
    class CameraSystem;

    class AbstractGraphicSystem : public IGraphicSystem {
    public:
        AbstractGraphicSystem(CameraSystem* camera);
        virtual ~AbstractGraphicSystem();
        void linkCamera(CameraSystem* camera) override;
        void setDrawables(DrawableContainer& container) override;

    protected:
        CameraSystem* m_camera;
        DrawableContainer* m_drawables;
        std::vector<PositionnedGraphicDrawable> m_pgd;
    };
}
