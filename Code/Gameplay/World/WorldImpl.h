#pragma once
#include "../../ska/World/World.h"

namespace ska {
	class IniReader;
	class PrefabEntityManager;
}

class WorldImpl : public ska::World {
	
public:
	WorldImpl(unsigned int tailleBloc, const unsigned int wWidth, const unsigned int wHeight);
	~WorldImpl();
	void load(std::string fileName, std::string chipsetName) override;
	void changeLevel(std::string fileName, std::string chipsetName) override;

	virtual void graphicUpdate(ska::DrawableContainer& drawables) override;


	
};