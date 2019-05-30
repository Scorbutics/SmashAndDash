#pragma once
#include "Script/System/ScriptBasicCommandsSystem.h"
#include "../../Gameplay/PokemonGameEventDispatcher.h"
#include "../../Gameplay/CustomEntityManager.h"

class WorldState;

namespace ska {
	class TileWorld;
	namespace cp {
		class Space;
	}
}

class ScriptCommandsSystem :
	public ska::ScriptBasicCommandsSystem
{
public:
	ScriptCommandsSystem(CustomEntityManager& entityManager, const ska::EntityLocator& locator, ska::TileWorld& w, ska::MemoryScript& saveGame, PokemonGameEventDispatcher& ged, ska::cp::Space& space);
	~ScriptCommandsSystem() override = default;

protected:
	struct ScriptCommandHelper : public BasicScriptCommandHelper {
		ScriptCommandHelper(ska::TileWorld& w, CustomEntityManager& e, const ska::EntityLocator& locator, PokemonGameEventDispatcher& ged, ska::cp::Space& space) :
		    BasicScriptCommandHelper(w, e, locator), m_ged(ged),
			m_world(w),
			m_customEntityManager(e),
			m_space(space) {
		}

		void setupCommands(std::unordered_map<std::string, ska::CommandPtr>& c) const override;
	private:
		ska::TileWorld& m_world;
		PokemonGameEventDispatcher& m_ged;
		CustomEntityManager& m_customEntityManager;
		ska::cp::Space& m_space;
	};

};

