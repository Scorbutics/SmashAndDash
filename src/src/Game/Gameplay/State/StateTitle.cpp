#include "Game/__internalConfig/LoggerConfig.h"
#include <algorithm>
#include <cmath>
#include "StateTitle.h"
#include "Core/Draw/DrawableContainer.h"
#include "Core/Draw/Renderer.h"
#include "../../Utils/IDs.h"
#include "../Data/SaveSlots.h"
#include "../World/Generation/Files.h"

namespace {
	const ska::Color Soft{ 92, 98, 118, 255 };
	constexpr int EntryWidth = 420;
	constexpr int EntryHeight = 52;
	constexpr int EntryGap = 10;
	constexpr std::size_t MaxSaves = 4;



	unsigned char Mix(unsigned char a, unsigned char b, double t) {
		return static_cast<unsigned char>(a + (b - a) * t);
	}

	Ui::TextStyle Style(unsigned int size, ska::Color color, int outline = 0) {
		auto style = Ui::TextStyle{};
		style.size = size;
		style.color = color;
		style.outline = outline;
		style.outlineColor = ska::Color{ 34, 40, 64, 255 };
		return style;
	}
}

StateTitle::StateTitle(PokemonGameEventDispatcher& ged, std::function<void(const std::string&)> start, const std::string& direct) :
	ska::SubObserver<ska::InputKeyEvent>(std::bind(&StateTitle::onKey, this, std::placeholders::_1), ged),
	ska::SubObserver<ska::InputMouseEvent>(std::bind(&StateTitle::onMouse, this, std::placeholders::_1), ged),
	ska::SubObserver<ska::GameEvent>(std::bind(&StateTitle::onGameEvent, this, std::placeholders::_1), ged),
	m_start(std::move(start)),
	m_layer([this](Ui::Canvas& canvas) { render(canvas); }, GUI_DEFAULT_DISPLAY_PRIORITY + 10) {
	m_hero.load("Sprites/Charsets/0.png");
	if (!direct.empty()) {
		play(direct);
		return;
	}
	for (const auto& slot : SaveSlots::List()) {
		if (m_entries.size() >= MaxSaves) {
			break;
		}
		m_entries.push_back(Entry{ "Continuer : " + slot.name, slot.map.empty() ? "Son monde n'est pas encore généré" : "Sur la carte " + slot.map, slot.name });
	}
	m_entries.push_back(Entry{ "Nouvelle partie", "Un nouveau monde, généré pour vous", "" });
	m_choices.setCount(m_entries.size());
}

StateTitle::~StateTitle() = default;

bool StateTitle::onGameEvent(ska::GameEvent& event) {
	if (event.getEventType() == ska::GameEventType::GAME_WINDOW_READY || event.getEventType() == ska::GameEventType::GAME_WINDOW_RESIZED) {
		m_screen = { static_cast<int>(event.windowWidth), static_cast<int>(event.windowHeight) };
	}
	return false;
}

bool StateTitle::onKey(ska::InputKeyEvent& event) {
	if (m_choices.keys(event.icm) && m_mode == Mode::Menu) {
		choose(m_entries[m_choices.selected()]);
	}
	return false;
}

bool StateTitle::onMouse(ska::InputMouseEvent& event) {
	auto boxes = std::vector<ska::Rectangle>{};
	for (auto i = 0u; i < m_entries.size(); i++) {
		boxes.push_back(entryBox(i));
	}
	if (m_choices.mouse(event.icm, boxes) && m_mode == Mode::Menu) {
		choose(m_entries[m_choices.selected()]);
	}
	return false;
}

void StateTitle::choose(const Entry& entry) {
	if (entry.save.empty()) {
		const auto name = SaveSlots::NewName();
		SaveSlots::Create(name, SaveSlots::NewSeed());
		play(name);
	} else {
		play(entry.save);
	}
}

void StateTitle::play(const std::string& save) {
	m_save = save;
	if (!SaveSlots::Exists(save)) {
		SaveSlots::Create(save, SaveSlots::NewSeed());
	}
	const auto seed = SaveSlots::WorldToGenerate(save);
	m_mode = Mode::Loading;
	m_framesShown = 0;
	if (seed) {
		m_seed = *seed;
		m_writer = std::make_unique<worldgen::WorldWriter>(*seed, SaveSlots::WorldPath(save), SaveSlots::WorldPath(save));
		SLOG(ska::LogLevel::Info) << "Title : the world of " << save << " (seed " << *seed << ") is generated";
	}
}

void StateTitle::onGraphicUpdate(unsigned int ellapsedTime, ska::DrawableContainer& drawables) {
	m_time += ellapsedTime;
	// Loading : a step of the generator per frame, once the loading screen has been shown
	if (m_mode == Mode::Loading && m_framesShown++ > 0) {
		try {
			if (m_writer != nullptr && m_writer->step()) {
				drawables.add(m_layer);
				return;
			}
			if (m_writer != nullptr) {
				SaveSlots::WorldGenerated(m_save, m_seed, m_writer->start());
				SLOG(ska::LogLevel::Info) << "Title : the world of " << m_save << " is generated";
			}
			m_mode = Mode::Started;
			m_start(m_save);
		} catch (std::exception& e) {
			m_error = e.what();
			m_mode = Mode::Failed;
			SLOG(ska::LogLevel::Error) << "Title : the world of " << m_save << " cannot be generated : " << m_error;
		}
	}
	drawables.add(m_layer);
}

ska::Rectangle StateTitle::entryBox(std::size_t index) const {
	const auto count = static_cast<int>(m_entries.size());
	const auto total = count * EntryHeight + (count - 1) * EntryGap;
	const auto top = std::max(m_screen.y * 46 / 100, m_screen.y - 60 - total);
	return ska::Rectangle{ (m_screen.x - EntryWidth) / 2 + 90, top + static_cast<int>(index) * (EntryHeight + EntryGap), EntryWidth, EntryHeight };
}

void StateTitle::render(Ui::Canvas& canvas) const {
	renderBackground(canvas);
	if (m_mode == Mode::Menu) {
		renderMenu(canvas);
	} else {
		renderLoading(canvas);
	}
}

void StateTitle::renderBackground(Ui::Canvas& canvas) const {
	const auto w = m_screen.x;
	const auto h = m_screen.y;
	// The sky : a gradient in bands, as the voxels
	const auto horizon = h * 62 / 100;
	for (auto y = 0; y < horizon; y += 8) {
		const auto t = static_cast<double>(y) / horizon;
		canvas.fill(ska::Color{ Mix(82, 196, t), Mix(156, 228, t), Mix(222, 246, t), 255 }, ska::Rectangle{ 0, y, w, 8 });
	}
	// Clouds drifting
	const auto drift = static_cast<int>(m_time / 60);
	for (auto i = 0; i < 5; i++) {
		const auto cw = 90 + (i * 37) % 70;
		const auto cx = ((i * 263 + drift * (1 + i % 2)) % (w + 2 * cw)) - cw;
		const auto cy = 40 + (i * 71) % (horizon / 2);
		canvas.fill(ska::Color{ 255, 255, 255, 255 }, ska::Rectangle{ cx, cy, cw, 18 });
		canvas.fill(ska::Color{ 255, 255, 255, 255 }, ska::Rectangle{ cx + cw / 4, cy - 12, cw / 2, 12 });
		canvas.fill(ska::Color{ 214, 230, 244, 255 }, ska::Rectangle{ cx, cy + 18, cw, 6 });
	}
	// Hills, then the meadow
	for (auto x = 0; x < w; x += 12) {
		const auto hill = static_cast<int>(28 * std::sin(x / 150.0) + 18 * std::sin(x / 61.0 + 1.3));
		canvas.fill(ska::Color{ 96, 160, 92, 255 }, ska::Rectangle{ x, horizon - 40 + hill, 12, 40 - hill + 6 });
	}
	canvas.fill(ska::Color{ 120, 186, 86, 255 }, ska::Rectangle{ 0, horizon, w, h - horizon });
	for (auto y = horizon; y < h; y += 24) {
		for (auto x = (y / 24 % 2) * 30; x < w; x += 60) {
			canvas.fill(ska::Color{ 104, 170, 76, 255 }, ska::Rectangle{ x, y + 6, 12, 6 });
		}
	}
	canvas.fill(ska::Color{ 84, 140, 66, 255 }, ska::Rectangle{ 0, horizon, w, 6 });
	// The hero, big, on the meadow (the standing frame, facing the player : Sprites/Charsets layout)
	const auto frame = ska::Rectangle{ 72, 2 * 72, 72, 72 };
	const auto bob = (m_time / 400) % 2 == 0 ? 0 : 3;
	canvas.image(m_hero, w / 2 - 250 - 36, h - 210 - bob, &frame, 3.F, 255, true);
	// The title
	canvas.text("Smash & Dash", Style(76, Ui::White, 3), w / 2, h * 9 / 100, Ui::Align::Center);
	canvas.text("Un monde nouveau à chaque partie", Style(24, ska::Color{ 255, 244, 200, 255 }, 3), w / 2, h * 9 / 100 + 100, Ui::Align::Center);
}

void StateTitle::renderMenu(Ui::Canvas& canvas) const {
	for (auto i = 0u; i < m_entries.size(); i++) {
		const auto box = entryBox(i);
		const auto selected = i == m_choices.selected();
		canvas.panel(selected ? "panel_small_teal.png" : "panel_small.png", box, 16, selected ? 255 : 235);
		canvas.text(m_entries[i].label, Style(20, selected ? Ui::White : Ui::Ink), box.x + 20, box.y + 6, Ui::Align::Left);
		canvas.text(m_entries[i].detail, Style(14, selected ? ska::Color{ 220, 246, 240, 255 } : Soft), box.x + 20, box.y + 30, Ui::Align::Left);
		if (selected) {
			canvas.text(">", Style(24, Ui::White), box.x + box.w - 30, box.y + 12, Ui::Align::Left);
		}
	}
	canvas.text("W / S : choisir    Entrée : valider", Style(14, Ui::White, 3), m_screen.x / 2 + 90, m_screen.y - 32, Ui::Align::Center);
}

void StateTitle::renderLoading(Ui::Canvas& canvas) const {
	const auto w = 520;
	const auto h = 132;
	const auto x = (m_screen.x - w) / 2 + 90;
	const auto y = m_screen.y * 50 / 100;
	canvas.panel("panel_small.png", ska::Rectangle{ x, y, w, h }, 16, 245);
	if (m_mode == Mode::Failed) {
		canvas.text("Le monde n'a pas pu être généré", Style(20, ska::Color{ 168, 40, 32, 255 }), x + w / 2, y + 22, Ui::Align::Center);
		canvas.text(m_error.substr(0, 70), Style(14, Soft), x + w / 2, y + 60, Ui::Align::Center);
		return;
	}
	const auto generating = m_writer != nullptr;
	canvas.text(generating ? "Génération du monde" : "Chargement", Style(22, Ui::Ink), x + w / 2, y + 16, Ui::Align::Center);
	if (generating) {
		canvas.text("Graine " + std::to_string(m_seed) + " · " + m_writer->stage(), Style(14, Soft), x + w / 2, y + 50, Ui::Align::Center);
	}
	// The progress bar
	auto bar = Ui::Bar{};
	bar.ratio = static_cast<float>(generating ? m_writer->progress() : 1.0);
	canvas.bar(ska::Rectangle{ x + 30, y + 82, w - 60, 20 }, bar);
}
