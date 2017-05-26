#pragma once
#include <unordered_map>
#include <memory>
#include "Point.h"
#include "Draw/Color.h"
#include "Chipset.h"
namespace ska {
	class Block;
	typedef std::unique_ptr<Block> BlockPtr;
	typedef std::unique_ptr<Chipset> ChipsetPtr;
	class SDLSurface;

	class ChipsetHolder {
	public:
		explicit ChipsetHolder(const std::string& correspondanceFile);
		ChipsetHolder(const ChipsetHolder&) = delete;
		ChipsetHolder& operator=(const ChipsetHolder&) = delete;

		bool attach(const unsigned int blockSize, const std::string& chipsetName);
		std::vector<ScriptSleepComponent*> getScript(const std::string& id, const ScriptTriggerType& type, bool& autoBlackList);
		void render(ska::Point<int> pos, const BlockRenderable& block) const;
		void update(BlockRenderable& block);
		const std::string& getName() const;
		void generateBlock(Color& key, Block** outputBlock, BlockRenderable** outputRenderable);
		~ChipsetHolder() = default;

	private:
		void buildCorrMap(const SDLSurface& fichierMCorr);
		std::unordered_map<Color, Point<int>> m_corr;
		ChipsetPtr m_chipset;
		unsigned int m_corrFileWidth;
		std::string m_currentChipsetName;
	};
}
