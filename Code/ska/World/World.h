#ifndef DEF_WORLD
#define DEF_WORLD

#include <iostream>
#include <string>
#include <unordered_set>
#include <memory>
#include <vector>
#include "../Inputs/Readers/IniReader.h"
#include "../Graphic/Texture.h"
#include "../Graphic/Animation.h"
#include "../Scene/Scene.h"
#include "../Graphic/System/CameraAware.h"
#include "ChipsetHolder.h"
#include "Layer.h"
#include "LayerE.h"

namespace ska {
	class CameraSystem;
	class PhysicObject;
	class Block;
	class PrefabEntityManager;
	class ScriptSleepComponent;
	typedef char ScriptTriggerType;

	class World : public HasGraphic, public CameraAware {
	public:
		World(const unsigned int tailleBloc, const unsigned int wWidth, const unsigned int wHeight);
		virtual void load(const std::string& fileName, const std::string& chipsetName);

		std::vector<IniReader>& getMobSettings();
		std::string getGenericName() const;
		std::string getName() const;
		std::string getFileName() const;

		unsigned int getPixelWidth() const;
		unsigned int getPixelHeight() const;
		
		void update();

		int getNbrBlocX() const;
		int getNbrBlocY() const;
		void setNbrBlocX(int nbrBlockX);
		void setNbrBlocY(int nbrBlockY);

		ska::LayerRenderable& getLayerRenderable(int level);
		LayerE& getLayerEvent();
		unsigned int getNumberLayers() const;

		const ska::Rectangle* getView() const;
		ska::ChipsetHolder& getChipset();

		Block* getHigherBlock(const unsigned int i, const unsigned int j) const;

		void getData();
		bool isSameBlockId(const ska::Point<int>& p1, const ska::Point<int>& p2, int layerIndex);
		bool canMoveToPos(ska::Rectangle pos, std::vector<ska::Point<int>>& output) const;
		bool canMoveOnBlock(const ska::Point<int>& pos, const std::unordered_set<int>& authorizedBlocks, int layerIndex) const;

		bool getCollision(const int i, const int j) const;
		bool isBlockDodgeable(const int i, const int j) const;

		const unsigned int getBlockSize() const;

		/* TODO classe à part ? */
		std::vector<ska::ScriptSleepComponent*> chipsetScript(const ska::Point<int>& oldPos, const ska::Point<int>& newPos, const ska::Point<int>& p, const ScriptTriggerType& reason, const unsigned int layerIndex);
		ska::Rectangle placeOnNearestPracticableBlock(const ska::Rectangle& hitBox, const unsigned int radius) const;
		ska::Point<int> alignOnBlock(const ska::Rectangle& hitbox) const;

		void linkCamera(CameraSystem* cs) override;
		virtual void graphicUpdate(DrawableContainer& drawables) = 0;

		virtual ~World() = default;

	private:
		void getRainFromData(std::string stringDataFile);
		void getMobSettingsFromData();

		int m_windDirection;
		int m_nbrBlockX, m_nbrBlockY;
		unsigned int m_blockSize;

		
		std::string m_fileName, m_genericName, m_worldName;
		
		bool m_autoScriptsPlayed;
		std::vector<IniReader> m_mobSettings;
		CameraSystem* m_cameraSystem;

	protected:
		Layer m_lBot;
		Layer m_lMid;
		Layer m_lTop;
		LayerE m_layerE;
		ChipsetHolder m_chipset;
	};
}

#endif
