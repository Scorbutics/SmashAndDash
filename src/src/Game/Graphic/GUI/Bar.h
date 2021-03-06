#pragma once

#include <iostream>
#include <string>
#include "Graphic/Texture.h"
#include "Core/Draw/DrawableFixedPriority.h"
#include "Core/ECS/ECSDefines.h"
class SpriteBank;

namespace ska {
	class EntityManager;
	class CameraSystem;
}

//HP Bar, PP Bar, etc...
class Bar : public ska::DrawableFixedPriority {
public:
	Bar(ska::CameraSystem& cam, const std::string& barStyleName, const std::string& barContentName, int maxValue, ska::EntityManager& em, const ska::EntityId& entityId);
	~Bar() override = default;

	Bar& operator=(const Bar&) = delete;

    void render(ska::Renderer& renderer) const override;
	bool isVisible() const;

	void setCurrentValue(unsigned int v);
    void setVisible(bool x);

private:
	ska::EntityManager& m_entityManager;
	const ska::EntityId& m_entityId;

	ska::CameraSystem& m_cameraSystem;
	ska::Texture m_barStyle;
	ska::Texture m_barContent;
	ska::Rectangle m_barSize;
    unsigned int m_maxValue;

	unsigned int m_curValue;
    bool m_visible;
};

using BarPtr = std::unique_ptr<Bar>;

