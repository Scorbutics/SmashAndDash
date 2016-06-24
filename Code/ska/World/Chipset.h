#pragma once
#include <unordered_map>
#include <memory>
#include "../Graphic/Color.h"
#include "../Graphic/Point.h"
#include "../Graphic/SDLSurface.h"
#include "../Graphic/Texture.h"
#include "../Script/ScriptSleepComponent.h"

namespace ska {
	class Block;
	typedef std::unique_ptr<Block> BlockPtr;
	typedef char ScriptTriggerType;
	

	class Chipset {
	public:
		Chipset(const std::unordered_map<ska::Color, ska::Point<int>>& corr, const unsigned int corrFileWidth, const int blockSize, const std::string& chipsetName);
		BlockPtr generateBlock(ska::Color& inputColor);
		const std::string& getName() const;
		std::vector<ska::ScriptSleepComponent*> getScript(const std::string& id, const ska::ScriptTriggerType& type, bool& autoBlackList);
		Texture& getTexture();
		~Chipset() = default;

	private:
		void load();
		void fillScript(const std::string& chipsetFolder, const std::string& id, const ska::ScriptTriggerType& type);

		const std::unordered_map<ska::Color, ska::Point<int>>& m_corr;
		std::unordered_map<std::string, ScriptSleepComponent> m_triggeredScripts;
		std::unordered_map<int, ScriptSleepComponent> m_autoScripts;
		const int m_blockSize;
		const unsigned int m_corrFileWidth;
		std::string m_chipsetName;
		SDLSurface m_sCol;
		SDLSurface m_sChipset;
		SDLSurface m_sProperties;
		Texture m_chipset;

		Uint16 m_darkColor;
		Uint16 m_whiteColor;
		Uint16 m_lightColor;
	};
}