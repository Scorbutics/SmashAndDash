#ifndef DEF_WORLD
#define DEF_WORLD

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include "../Inputs/Readers/IniReader.h"
#include "../Graphic/Texture.h"
#include "../Graphic/Animation.h"
#include "../Scene/Scene.h"
#include "../Graphic/System/CameraSystem.h"
#include "../ECS/EntityManager.h"

namespace ska {
	class Layer;
	class LayerE;
	class PhysicObject;
	class Block;

	class World : public HasGraphic {
	public:
		World(const unsigned int tailleBloc, const unsigned int wWidth, const unsigned int wHeight);
		virtual void load(std::string fileName, std::string chipsetName, std::string saveName);

		/*Weather* getFog();
		Weather* getWeather();*/
		std::vector<IniReader>& getMobSettings();
		std::string getGenericName();
		int getNbrBlocX();
		unsigned int getPixelWidth() const;
		unsigned int getPixelHeight() const;
		int getNbrBlocY();
		std::unique_ptr<ska::Layer>& getLayerBot();
		std::unique_ptr<ska::Layer>& getLayerMid();
		std::unique_ptr<ska::Layer>& getLayerTop();
		const ska::Rectangle* getView() const;
		std::unique_ptr<ska::LayerE>& getLayerEvent();
		void setWind(int wind);
		ska::Texture& getChipset();
		int getWind() const;
		std::string getChipsetName();
		std::string getName();
		void getData();
		bool getCollision(const int i, const int j);
		bool isBlockDodgeable(const int i, const int j);

		unsigned int getBlockSize() const;

		void setNbrBlocX(int nbrBlockX);
		void setNbrBlocY(int nbrBlockY);
		void setSpriteFrame(unsigned int x);
		void setBgmVolume(int volPercent);
		void playBgm(bool x);
		void setBgm(std::string bgm);
		bool isBgmPlaying();
		bool canMoveToPos(ska::Point<int> pos);

		Block* getHigherBlock(const unsigned int i, const unsigned int j);
		void changeLevel(std::string fileName, std::string chipsetName);
		int spawnMob(ska::Rectangle pos, unsigned int rmin, unsigned int rmax, unsigned int nbrSpawns, IniReader* dataSpawn);

		virtual void refreshEntities() = 0;
		virtual void graphicUpdate(DrawableContainer& drawables) = 0;

		ska::EntityManager& getEntityManager();

		~World();

	private:
		void getFogFromData(std::string stringDataFile);
		void getRainFromData(std::string stringDataFile);
		void getWeatherFromData(std::string stringDataFile);
		void getBgmFromData(std::string stringDataFile);
		void getMobSettingsFromData();

		int m_windDirection;
		int m_nbrBlockX, m_nbrBlockY;
		unsigned int m_blockSize;

		Texture m_chipset;
		std::string m_chipsetName, m_fileName, m_genericName, m_worldName, m_botLayerName, m_midLayerName, m_topLayerName, m_eventLayerName, m_bgmName;
		//std::unique_ptr<Weather> m_temps, m_brouillard;
		std::vector<IniReader> m_mobSettings;

	protected:
		std::unique_ptr<Layer> m_lBot, m_lMid, m_lTop;
		std::unique_ptr<LayerE> m_lEvent;
		Animation m_animBlocks;
		EntityManager m_entityManager;
		CameraSystem m_cameraSystem;
	};
}

#endif
