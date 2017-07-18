#pragma once
#include <string>
#include "Color.h"

namespace ska {
    class SDLRenderer;

	class TextureData {
	public:
		TextureData(SDLRenderer& r, const std::string&, Color c);
        TextureData();

		std::pair<std::string, Color> getData() const;
		const SDLRenderer& getRenderer() const;

		virtual ~TextureData();

	private:
		std::pair<std::string, Color> m_data;
		SDLRenderer* m_renderer;
	};
}


namespace std {
	template <>
	struct hash<ska::TextureData> {
		size_t operator()(const ska::TextureData& k) const {
			using std::size_t;
			using std::hash;

			auto d = k.getData();
			size_t hashSeed = hash<string>()(d.first);
			ska::NumberUtils::hashCombine<ska::Color>(hashSeed, d.second);
			return hashSeed;
		}
	};

	template <>
	struct equal_to<ska::TextureData> {
		bool operator()(const ska::TextureData& t, const ska::TextureData& k) const {
			return t.getData() == k.getData();
		}
	};
}
