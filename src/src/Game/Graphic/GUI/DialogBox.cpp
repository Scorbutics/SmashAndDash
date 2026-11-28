#include "Game/__internalConfig/LoggerConfig.h"
#include "DialogBox.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "Core/Data/Events/ScriptEvent.h"
#include "Core/Draw/DrawableContainer.h"
#include "Core/Inputs/InputContextManager.h"
#include "Core/Inputs/InputAction.h"
#include "Core/Inputs/InputRange.h"
#include "Graphic/System/CameraSystem.h"
#include "../../Gameplay/CustomEntityManager.h"
#include "../../Gameplay/Control/PlayerControls.h"
#include "../../Script/ScriptConstants.h"
#include "../../Gameplay/Data/DialogEvent.h"
#include "../../Gameplay/World/WorldState.h"
#include "../../Utils/IDs.h"

namespace {
	constexpr unsigned int TextSize = 22;
	constexpr int LineHeight = 31;

	Ui::TextStyle Style(unsigned int size, ska::Color color, float alpha = 255.F) {
		auto style = Ui::TextStyle{};
		style.size = size;
		style.color = color;
		style.alpha = static_cast<unsigned char>(std::clamp(alpha, 0.F, 255.F));
		return style;
	}

	bool Inside(const ska::Rectangle& box, const ska::Point<int>& point) {
		return box.w > 0 && point.x >= box.x && point.x < box.x + box.w && point.y >= box.y && point.y < box.y + box.h;
	}

	// The first "count" letters of a text (UTF-8 : a letter is not cut)
	std::string Letters(const std::string& text, std::size_t count) {
		auto end = std::size_t{ 0 };
		auto seen = std::size_t{ 0 };
		while (end < text.size()) {
			if ((static_cast<unsigned char>(text[end]) & 0xC0) != 0x80) {
				if (seen == count) {
					break;
				}
				seen++;
			}
			end++;
		}
		return text.substr(0, end);
	}

	std::size_t Count(const std::string& text) {
		return static_cast<std::size_t>(std::count_if(text.begin(), text.end(), [](char c) { return (static_cast<unsigned char>(c) & 0xC0) != 0x80; }));
	}
}

DialogBox::DialogBox(CustomEntityManager& em, PokemonGameEventDispatcher& ged, WorldState& worldState) :
	ska::SubObserver<DialogEvent>(std::bind(&DialogBox::onDialog, this, std::placeholders::_1), ged),
	ska::SubObserver<ska::InputKeyEvent>(std::bind(&DialogBox::onKey, this, std::placeholders::_1), ged),
	ska::SubObserver<ska::InputMouseEvent>(std::bind(&DialogBox::onMouse, this, std::placeholders::_1), ged),
	m_entities(em),
	m_ged(ged),
	m_worldState(worldState),
	m_layer([this](Ui::Canvas& canvas) { render(canvas); }, GUI_DEFAULT_DISPLAY_PRIORITY + 5),
	// Nobody reads with SAD_BATTLE_AUTOPLAY : the text is written at once (the dialogs close by themselves)
	m_instant(std::getenv("SAD_BATTLE_AUTOPLAY") != nullptr) {
}

ska::Rectangle DialogBox::box(int lines) const {
	const auto screen = m_worldState.getCamera().getScreenSize();
	const auto width = std::min(screen.x - 64, 960);
	const auto height = std::max(118, 46 + lines * LineHeight);
	return ska::Rectangle{ (screen.x - width) / 2, screen.y - height - 22, width, height };
}

bool DialogBox::onDialog(DialogEvent& event) {
	auto dialog = Shown{};
	dialog.text = event.message;
	dialog.question = event.question;
	dialog.onClose = event.onClose;
	if (event.callbackActive) {
		dialog.script = event.callback;
	}
	if (m_current.has_value()) {
		m_waiting.push_back(std::move(dialog));
	} else {
		// Right after another one : it takes its box
		show(std::move(dialog), m_closedAge.has_value());
	}
	return true;
}

void DialogBox::show(Shown dialog, bool chained) {
	const auto width = static_cast<std::size_t>(std::max(20, (box(1).w - 60) / 11));
	m_lines = Ui::Wrap(dialog.text, width);
	m_current = std::move(dialog);
	m_age = 0;
	m_complete = m_instant;
	m_chained = chained;
	m_answer = 0;
	m_closedAge.reset();
	if (!m_heldTrainer.has_value()) {
		if (const auto* trainer = m_worldState.getEntityLocator().getEntityId(SCRIPT_ENTITY_TRAINER); trainer != nullptr) {
			HoldPlayerControls(m_entities, *trainer);
			m_heldTrainer = *trainer;
		}
	}
	m_worldState.setDialogOpen(true);
}

void DialogBox::next() {
	if (!m_waiting.empty()) {
		auto dialog = std::move(m_waiting.front());
		m_waiting.pop_front();
		show(std::move(dialog), true);
	}
}

std::size_t DialogBox::letters() const {
	auto count = std::size_t{ 0 };
	for (const auto& line : m_lines) {
		count += Count(line);
	}
	return count;
}

bool DialogBox::written() const {
	return m_complete || m_age / LetterTime >= letters();
}

void DialogBox::close(bool validated) {
	if (!m_current.has_value()) {
		return;
	}
	auto closed = std::move(*m_current);
	m_current.reset();
	m_closedLines = m_lines;
	m_closedAge = 0;
	// The scripts go on now : the next dialog comes while this box is still shown
	if (closed.onClose) {
		closed.onClose(validated);
	}
	if (closed.script.has_value()) {
		const auto scriptEntity = m_entities.createEntity();
		auto events = std::vector<ska::ScriptEventType>{ ska::ScriptEventType::ScriptCreate };
		auto event = ska::ScriptEvent{ std::move(events), std::move(*closed.script), scriptEntity, {}, {}, {}, {} };
		m_ged.ska::Observable<ska::ScriptEvent>::notifyObservers(event);
	}
	next();
}

void DialogBox::refresh(unsigned int ellapsedTime) {
	m_time += ellapsedTime;
	m_age += ellapsedTime;
	if (m_closedAge.has_value()) {
		*m_closedAge += ellapsedTime;
		// No dialog after it : the trainer moves again, the box slides out
		if (*m_closedAge >= ChainDelay && m_worldState.dialogOpen()) {
			m_worldState.setDialogOpen(false);
			if (m_heldTrainer.has_value()) {
				ReleasePlayerControls(m_entities, *m_heldTrainer);
				m_heldTrainer.reset();
			}
		}
		if (*m_closedAge >= ChainDelay + SlideTime) {
			m_closedAge.reset();
			m_closedLines.clear();
		}
	}
}

void DialogBox::graphicUpdate(ska::DrawableContainer& drawables) {
	drawables.add(m_layer);
}

bool DialogBox::onKey(ska::InputKeyEvent& event) {
	const auto& toggles = event.icm.getToggles();
	const auto& actions = event.icm.getActions();
	const auto confirmKey = actions[ska::DoAction];
	const auto backKey = actions[ska::Cancel] || actions[ska::DeleteChar];
	const auto confirmed = confirmKey && !m_lastConfirm;
	const auto backed = backKey && !m_lastBack;
	const auto pressed = [&](ska::InputToggle toggle) { return toggles[toggle] && !m_lastToggles[toggle]; };
	if (m_current.has_value()) {
		if (m_current->question && written() && (pressed(ska::MoveLeft) || pressed(ska::MoveRight) || pressed(ska::MoveUp) || pressed(ska::MoveDown))) {
			m_answer = 1 - m_answer;
		}
		if (confirmed) {
			if (!written()) {
				m_complete = true;
			} else {
				close(!m_current->question || m_answer == 0);
			}
		} else if (backed && m_current->question) {
			close(false);
		}
	}
	m_lastToggles = toggles;
	m_lastConfirm = confirmKey;
	m_lastBack = backKey;
	return true;
}

bool DialogBox::onMouse(ska::InputMouseEvent& event) {
	const auto& position = event.icm.getRanges()[ska::MousePos];
	const auto click = event.icm.getActions()[ska::LClic];
	const auto clicked = click && !m_lastClick;
	m_lastClick = click;
	if (!m_current.has_value() || (position.x == 0 && position.y == 0)) {
		return true;
	}
	const auto at = ska::Point<int>{ static_cast<int>(position.x), static_cast<int>(position.y) };
	if (m_current->question && written()) {
		if (Inside(m_yes, at)) {
			m_answer = 0;
		} else if (Inside(m_no, at)) {
			m_answer = 1;
		}
	}
	if (clicked && (Inside(m_box, at) || Inside(m_yes, at) || Inside(m_no, at))) {
		if (!written()) {
			m_complete = true;
		} else {
			close(!m_current->question || m_answer == 0);
		}
	}
	return true;
}

void DialogBox::render(Ui::Canvas& canvas) const {
	m_box = {};
	m_yes = {};
	m_no = {};
	const auto shown = m_current.has_value();
	const auto& lines = shown ? m_lines : m_closedLines;
	if (!shown && !m_closedAge.has_value()) {
		return;
	}
	auto area = box(static_cast<int>(lines.size()));
	// It slides in (not after another one), out (no other one after it)
	auto slide = 0.F;
	if (shown && !m_chained) {
		slide = 1.F - std::min(1.F, static_cast<float>(m_age) / static_cast<float>(SlideTime));
	} else if (!shown && *m_closedAge > ChainDelay) {
		slide = std::min(1.F, static_cast<float>(*m_closedAge - ChainDelay) / static_cast<float>(SlideTime));
	}
	const auto ease = slide * slide;
	area.y += static_cast<int>(ease * static_cast<float>(area.h + 30));
	canvas.panel("panel_small.png", area, 18, 250);
	canvas.fill(Ui::Teal, ska::Rectangle{ area.x + 18, area.y + 6, area.w - 36, 3 });
	m_box = area;

	// The text, letter by letter
	auto left = shown ? (written() ? std::size_t{ 1000000 } : static_cast<std::size_t>(m_age / LetterTime)) : std::size_t{ 1000000 };
	for (auto i = std::size_t{ 0 }; i < lines.size(); i++) {
		const auto count = Count(lines[i]);
		const auto part = left >= count ? lines[i] : Letters(lines[i], left);
		left = left >= count ? left - count : 0;
		canvas.text(part, Style(TextSize, Ui::Ink), area.x + 30, area.y + 22 + static_cast<int>(i) * LineHeight, Ui::Align::Left);
	}
	if (!shown || !written()) {
		return;
	}
	if (m_current->question) {
		// Oui / Non, over the right of the box
		const auto pill = [&](const ska::Rectangle& r, const std::string& label, bool selected) {
			canvas.panel(selected ? "panel_small_teal.png" : "panel_small.png", r, 12, 250);
			canvas.text(label, Style(19, selected ? Ui::White : Ui::Ink), r.x + r.w / 2, r.y + 9, Ui::Align::Center);
			if (selected) {
				const auto bob = static_cast<int>(3.F * std::sin(static_cast<float>(m_time) / 120.F));
				canvas.text(">", Style(19, Ui::White), r.x + 12 + bob, r.y + 9, Ui::Align::Left);
			}
		};
		m_yes = ska::Rectangle{ area.x + area.w - 250, area.y - 52, 116, 44 };
		m_no = ska::Rectangle{ area.x + area.w - 126, area.y - 52, 116, 44 };
		pill(m_yes, "Oui", m_answer == 0);
		pill(m_no, "Non", m_answer == 1);
		return;
	}
	// The arrow : more to read
	if ((m_time / 350) % 2 == 0) {
		const auto x = area.x + area.w - 40;
		const auto y = area.y + area.h - 26;
		for (auto row = 0; row < 6; row++) {
			canvas.fill(Ui::Ink, ska::Rectangle{ x + row, y + row, 12 - 2 * row, 1 });
		}
	}
}
