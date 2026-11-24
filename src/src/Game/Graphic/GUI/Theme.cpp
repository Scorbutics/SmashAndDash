#include <algorithm>
#include <map>
#include <vector>
#include "Theme.h"
#include "GUI/GUI.h"
#include "Graphic/Texture.h"
#include "Core/Draw/Renderer.h"

const ska::Texture& Theme::image(const std::string& name) {
	static auto images = std::map<std::string, ska::Texture>{};
	auto found = images.find(name);
	if (found == images.end()) {
		found = images.emplace(name, ska::Texture{ ska::GUI::MENU_DEFAULT_THEME_PATH + name }).first;
	}
	return found->second;
}

namespace {
	struct Span {
		int source;
		int destination;
		int length;
	};

	// The pieces of one axis : the first corner, the middle repeated, the last corner
	std::vector<Span> Spans(int from, int length, int corner, int sourceLength) {
		auto spans = std::vector<Span>{};
		const auto to = from + length;
		const auto middle = sourceLength - 2 * corner;
		spans.push_back(Span{ 0, from, corner });
		for (auto position = from + corner; middle > 0 && position < to - corner; position += middle) {
			spans.push_back(Span{ corner, position, std::min(middle, to - corner - position) });
		}
		spans.push_back(Span{ sourceLength - corner, to - corner, corner });
		return spans;
	}
}

void Theme::panel(ska::Renderer& renderer, const std::string& name, const ska::Rectangle& box, int corner, unsigned char alpha) {
	if (box.w <= 0 || box.h <= 0) {
		return;
	}
	const auto& texture = image(name);
	const auto width = static_cast<int>(texture.getWidth());
	const auto height = static_cast<int>(texture.getHeight());
	const auto c = std::max(1, std::min({ corner, box.w / 2, box.h / 2, width / 2, height / 2 }));
	auto effect = ska::RenderEffect{};
	effect.alpha = alpha;
	renderer.setEffect(&effect);
	for (const auto& row : Spans(box.y, box.h, c, height)) {
		for (const auto& column : Spans(box.x, box.w, c, width)) {
			if (row.length > 0 && column.length > 0) {
				const auto clip = ska::Rectangle{ column.source, row.source, column.length, row.length };
				renderer.render(texture, column.destination, row.destination, &clip);
			}
		}
	}
	renderer.setEffect(nullptr);
}
