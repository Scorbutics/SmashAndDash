#include "Game/__internalConfig/LoggerConfig.h"
#include "DebugPanel.h"
#include <algorithm>
#include <cstdlib>
#include "Core/Draw/DrawableContainer.h"
#include "Core/Inputs/InputContextManager.h"
#include "Core/Inputs/InputAction.h"
#include "../../Gameplay/World/WorldState.h"
#include "../../Utils/IDs.h"

namespace {
	Ui::TextStyle Style(unsigned int size, ska::Color color) {
		auto style = Ui::TextStyle{};
		style.size = size;
		style.color = color;
		return style;
	}
}

DebugPanel::DebugPanel(PokemonGameEventDispatcher& ged, WorldState& worldState) :
	ska::SubObserver<ska::DebugGUIEvent>(std::bind(&DebugPanel::onDebug, this, std::placeholders::_1), ged),
	ska::SubObserver<ska::InputKeyEvent>(std::bind(&DebugPanel::onKey, this, std::placeholders::_1), ged),
	m_worldState(worldState),
	m_layer([this](Ui::Canvas& canvas) { render(canvas); }, GUI_DEFAULT_DISPLAY_PRIORITY + 10),
	m_shown(std::getenv("SAD_DEBUG") != nullptr && std::string{ std::getenv("SAD_DEBUG") } != "0") {
}

bool DebugPanel::onDebug(ska::DebugGUIEvent& event) {
	if (event.type == ska::DebugGUIEventType::REMOVE_DEBUG_INFO) {
		m_lines.erase(event.text);
		m_order.erase(std::remove(m_order.begin(), m_order.end(), event.text), m_order.end());
		return true;
	}
	if (event.type == ska::DebugGUIEventType::ADD_DEBUG_INFO && event.refresher) {
		if (m_lines.find(event.text) == m_lines.end()) {
			m_order.push_back(event.text);
		}
		auto& line = m_lines[event.text];
		line.label = event.text;
		line.read = std::move(event.refresher);
		line.delay = event.delay;
		line.age = line.delay;
	}
	return true;
}

bool DebugPanel::onKey(ska::InputKeyEvent& event) {
	const auto key = event.icm.getActions()[ska::Debug];
	if (key && !m_lastKey) {
		m_shown = !m_shown;
	}
	m_lastKey = key;
	return true;
}

void DebugPanel::refresh(unsigned int ellapsedTime) {
	if (!m_shown) {
		return;
	}
	for (auto& [name, line] : m_lines) {
		line.age += ellapsedTime;
		if (line.age >= line.delay && line.read) {
			line.age = 0;
			line.value = line.read();
		}
	}
}

void DebugPanel::graphicUpdate(ska::DrawableContainer& drawables) {
	if (m_shown) {
		drawables.add(m_layer);
	}
}

void DebugPanel::render(Ui::Canvas& canvas) const {
	auto rows = std::vector<std::pair<std::string, std::string>>{};
	rows.emplace_back("Carte", m_worldState.getMapName());
	for (const auto& name : m_order) {
		const auto found = m_lines.find(name);
		if (found != m_lines.end()) {
			auto label = found->second.label;
			// "FPS : " -> "FPS"
			label.erase(label.find_last_not_of(" :") + 1);
			rows.emplace_back(label, found->second.value);
		}
	}
	constexpr int RowHeight = 20;
	const auto width = 600;
	const auto height = 38 + static_cast<int>(rows.size()) * RowHeight;
	const auto x = 12;
	const auto y = 12;
	canvas.fill(ska::Color{ 10, 14, 30, 200 }, ska::Rectangle{ x, y, width, height });
	canvas.fill(Ui::Teal, ska::Rectangle{ x, y, width, 3 });
	canvas.text("DEBUG", Style(14, Ui::Teal), x + 12, y + 8, Ui::Align::Left);
	canvas.text("F3 : masquer", Style(12, Ui::Faded), x + width - 12, y + 10, Ui::Align::Right);
	for (auto i = std::size_t{ 0 }; i < rows.size(); i++) {
		const auto rowY = y + 32 + static_cast<int>(i) * RowHeight;
		canvas.text(rows[i].first, Style(13, ska::Color{ 160, 200, 230, 255 }), x + 12, rowY, Ui::Align::Left);
		// A long value is cut to the panel
		auto value = rows[i].second.empty() ? std::string{ "-" } : rows[i].second;
		if (value.size() > 64) {
			value = value.substr(0, 62) + "..";
		}
		canvas.text(value, Style(13, Ui::White), x + 130, rowY, Ui::Align::Left);
	}
}
