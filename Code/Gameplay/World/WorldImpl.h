#pragma once
#include "../../ska/World/World.h"

class WorldImpl : public ska::World {
public:
	WorldImpl(const unsigned int tailleBloc, ska::CameraSystem& camera);
	~WorldImpl();
	virtual void refreshEntities() override;
	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;
};