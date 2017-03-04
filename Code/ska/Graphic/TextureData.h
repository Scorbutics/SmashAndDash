#pragma once
#include <string>
#include "Color.h"

namespace ska {
	class Window;
	class TextureData {
	public:
		TextureData(Window& window, std::string, Color c);
		TextureData();

		void operator=(const TextureData& t2);

		std::pair<std::string, Color> getData() const;
		const Window& getWindow() const;

		virtual ~TextureData();

	private:
		std::pair<std::string, Color> m_data;
		Window* m_window;
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
