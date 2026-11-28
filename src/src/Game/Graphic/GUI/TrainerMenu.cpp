#include "Game/__internalConfig/LoggerConfig.h"
#include "TrainerMenu.h"
#include <algorithm>
#include <cmath>
#include "Base/IO/Readers/IniReader.h"
#include "Core/Draw/DrawableContainer.h"
#include "Core/Draw/Renderer.h"
#include "Core/Inputs/InputContextManager.h"
#include "Core/Inputs/InputAction.h"
#include "Core/Inputs/InputRange.h"
#include "Core/Utils/SpritePath.h"
#include "Core/Utils/SkaConstants.h"
#include "Graphic/System/CameraSystem.h"
#include "../../Gameplay/CustomEntityManager.h"
#include "../../Gameplay/Control/PlayerControls.h"
#include "../../Gameplay/Data/Bag.h"
#include "../../Gameplay/Data/ItemUse.h"
#include "../../Gameplay/Data/Settings.h"
#include "../../Gameplay/Data/Team.h"
#include "../../Gameplay/Fight/BattleRules.h"
#include "../../Gameplay/Fight/Moveset.h"
#include "../../Gameplay/World/WorldState.h"
#include "../../Script/ScriptConstants.h"
#include "../../Utils/IDs.h"

namespace {
	const ska::Color Ink = Ui::Ink;
	const ska::Color White = Ui::White;
	const ska::Color Soft{ 96, 104, 120, 255 };
	const ska::Color Gold{ 255, 214, 90, 255 };
	const char* TabNames[] = { "Équipe", "Sac", "Options", "Fermer" };
	const char* TabDetails[] = { "Vos Pokémon", "Vos objets", "Le son, l'affichage", "Retour au jeu" };
	const char* ActionNames[] = { "Résumé", "Déplacer", "Retour" };
	const char* OptionNames[] = { "Volume général", "Musique", "Effets sonores", "Météo", "Brouillard" };
	constexpr int Tabs = 4;
	constexpr int Actions = 3;
	constexpr int Options = 5;

	Ui::TextStyle Style(unsigned int size, ska::Color color, int outline = 0, float alpha = 255.F) {
		auto style = Ui::TextStyle{};
		style.size = size;
		style.color = color;
		style.outline = outline;
		style.alpha = static_cast<unsigned char>(std::clamp(alpha, 0.F, 255.F));
		return style;
	}

	ska::Color TypeColor(const std::string& type) {
		if (type == "Feu") { return ska::Color{ 236, 92, 48, 255 }; }
		if (type == "Eau") { return ska::Color{ 64, 130, 236, 255 }; }
		if (type == "Plante") { return ska::Color{ 78, 178, 76, 255 }; }
		if (type == "Electrique") { return ska::Color{ 222, 180, 20, 255 }; }
		if (type == "Vol") { return ska::Color{ 120, 150, 230, 255 }; }
		if (type == "Poison") { return ska::Color{ 160, 70, 170, 255 }; }
		if (type == "Sol") { return ska::Color{ 190, 150, 70, 255 }; }
		if (type == "Insecte") { return ska::Color{ 150, 180, 40, 255 }; }
		if (type == "Tenebres") { return ska::Color{ 90, 70, 80, 255 }; }
		if (type == "Dragon") { return ska::Color{ 100, 60, 220, 255 }; }
		if (type == "Acier") { return ska::Color{ 150, 160, 180, 255 }; }
		return ska::Color{ 150, 140, 120, 255 };
	}

	bool Inside(const ska::Rectangle& box, const ska::Point<int>& point) {
		return point.x >= box.x && point.x < box.x + box.w && point.y >= box.y && point.y < box.y + box.h;
	}

	// A type as a pill
	int TypePill(const Ui::Canvas& canvas, const std::string& type, int x, int y) {
		if (type.empty() || type == "Aucun") {
			return 0;
		}
		auto label = type;
		std::transform(label.begin(), label.end(), label.begin(), [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
		const auto width = 16 + static_cast<int>(label.size()) * 8;
		canvas.fill(TypeColor(type), ska::Rectangle{ x, y, width, 20 });
		canvas.fill(ska::Color{ 255, 255, 255, 60 }, ska::Rectangle{ x, y, width, 6 });
		canvas.text(label, Style(12, White), x + width / 2, y + 3, Ui::Align::Center);
		return width + 6;
	}
}

TrainerMenu::TrainerMenu(CustomEntityManager& em, PokemonGameEventDispatcher& ged, WorldState& worldState, Settings& settings) :
	ska::SubObserver<ska::InputKeyEvent>(std::bind(&TrainerMenu::onKey, this, std::placeholders::_1), ged),
	ska::SubObserver<ska::InputMouseEvent>(std::bind(&TrainerMenu::onMouse, this, std::placeholders::_1), ged),
	ska::SubObserver<InterfaceEvent>(std::bind(&TrainerMenu::onInterface, this, std::placeholders::_1), ged),
	m_entities(em),
	m_worldState(worldState),
	m_settings(settings),
	m_layer([this](Ui::Canvas& canvas) { render(canvas); }, GUI_DEFAULT_DISPLAY_PRIORITY - 1),
	m_ball("./Sprites/Fight/pokeball.png") {
}

ska::Point<int> TrainerMenu::screen() const {
	return m_worldState.getCamera().getScreenSize();
}

bool TrainerMenu::available() const {
	return !m_hidden && m_worldState.battleStage() == nullptr && !m_worldState.dialogOpen() && m_worldState.getEntityLocator().getEntityId(SCRIPT_ENTITY_TRAINER) != nullptr;
}

bool TrainerMenu::onInterface(InterfaceEvent& event) {
	m_hidden = event.hidden;
	if (m_hidden) {
		close();
	}
	return true;
}

void TrainerMenu::open(Mode mode) {
	if (!available()) {
		return;
	}
	if (m_mode == Mode::Closed) {
		m_openTime = 0;
		// The trainer waits : it does not walk, nor talk nor open (its controls are held)
		if (const auto* trainer = m_worldState.getEntityLocator().getEntityId(SCRIPT_ENTITY_TRAINER); trainer != nullptr) {
			HoldPlayerControls(m_entities, *trainer);
			m_heldTrainer = *trainer;
		}
		m_worldState.setMenuOpen(true);
	}
	reload();
	m_mode = mode;
	m_tab = mode == Mode::Bag ? 1 : (mode == Mode::Options ? 2 : (mode == Mode::Team ? 0 : m_tab));
}

void TrainerMenu::close() {
	if (m_mode == Mode::Closed) {
		return;
	}
	m_mode = Mode::Closed;
	m_movingFrom = -1;
	if (m_heldTrainer.has_value()) {
		ReleasePlayerControls(m_entities, *m_heldTrainer);
		m_heldTrainer.reset();
	}
	m_worldState.setMenuOpen(false);
}

void TrainerMenu::reload() {
	auto& rules = m_worldState.getBattleRules();
	const auto team = Team{ m_worldState.getSaveGame().getPathName() };
	m_team.clear();
	for (auto index = 0; index < team.size(); index++) {
		const auto pokemon = team.member(index);
		if (!pokemon.has_value()) {
			continue;
		}
		const auto data = ska::IniReader{ "./Data/Monsters/" + std::to_string(pokemon->id) + ".ini" };
		auto member = Member{};
		member.index = index;
		member.id = pokemon->id;
		member.name = data.exists("Description name") ? data.get<std::string>("Description name") : "?";
		member.type1 = data.exists("Description type1") ? data.get<std::string>("Description type1") : "";
		member.type2 = data.exists("Description type2") ? data.get<std::string>("Description type2") : "";
		const auto needed = data.exists("Experience level_1_exp_needed") ? data.get<int>("Experience level_1_exp_needed") : 0;
		member.level = rules.level(pokemon->experience, needed);
		member.maxHp = std::max(1, rules.maxHp(data.exists("BaseStats hp") ? data.get<int>("BaseStats hp") : 10, member.level));
		member.hp = std::clamp(pokemon->hp, 0, member.maxHp);
		const auto start = rules.levelExperience(member.level, needed);
		const auto next = rules.levelExperience(member.level + 1, needed);
		member.experience = next > start ? std::clamp(static_cast<float>(pokemon->experience - start) / static_cast<float>(next - start), 0.F, 1.F) : 0.F;
		const auto learnable = LearnableSkills(data);
		for (const auto skill : TeamMoveset(m_worldState, index)) {
			const auto found = std::find_if(learnable.begin(), learnable.end(), [skill](const LearnableSkill& s) { return s.index == skill; });
			if (found == learnable.end()) {
				continue;
			}
			member.skills.push_back(found->name);
			auto reader = ska::IniReader{};
			member.skillTypes.push_back(reader.load(found->file) && reader.exists("Description type") ? reader.get<std::string>("Description type") : "");
		}
		m_team.push_back(std::move(member));
	}
	m_leadId = m_team.empty() ? -1 : m_team.front().id;
	auto& bag = m_worldState.getBag();
	m_items.clear();
	for (const auto id : bag.items()) {
		const auto data = Bag::data(id);
		auto item = Item{};
		item.id = id;
		item.amount = bag.count(id);
		item.name = data.has_value() ? data->name : "?";
		item.description = data.has_value() ? data->description : "";
		item.heal = data.has_value() && data->heal > 0;
		item.capture = data.has_value() && data->capture;
		m_items.push_back(std::move(item));
	}
	m_money = bag.money();
	m_teamCursor = std::clamp(m_teamCursor, 0, std::max(0, static_cast<int>(m_team.size()) - 1));
	m_bagCursor = std::clamp(m_bagCursor, 0, std::max(0, static_cast<int>(m_items.size()) - 1));
}

void TrainerMenu::notice(std::string text) {
	m_notice = std::move(text);
	m_noticeAge = 0;
}

void TrainerMenu::refresh(unsigned int ellapsedTime) {
	m_time += ellapsedTime;
	m_openTime += ellapsedTime;
	m_noticeAge += ellapsedTime;
	// A battle (or a hidden interface) : the menu closes
	if (m_mode != Mode::Closed && !available()) {
		close();
	}
	// The card of the team : the first Pokemon (it changes : a capture, a script)
	m_leadCheck += ellapsedTime;
	if (m_mode == Mode::Closed && m_leadCheck >= 1500) {
		m_leadCheck = 0;
		const auto first = Team{ m_worldState.getSaveGame().getPathName() }.member(0);
		m_leadId = first.has_value() ? first->id : -1;
	}
	// A choice asked by a script
	const auto& prompt = m_worldState.prompt();
	if (prompt.active && !m_promptShown) {
		m_promptShown = true;
		m_promptChoice = Ui::ChoiceList{};
		m_promptChoice.setCount(prompt.choices.size());
	} else if (!prompt.active) {
		m_promptShown = false;
	}
}

void TrainerMenu::graphicUpdate(ska::DrawableContainer& drawables) {
	drawables.add(m_layer);
}

bool TrainerMenu::onKey(ska::InputKeyEvent& event) {
	const auto& toggles = event.icm.getToggles();
	const auto& actions = event.icm.getActions();
	const auto pressed = [&](ska::InputToggle toggle) { return toggles[toggle] && !m_lastToggles[toggle]; };
	const auto confirmKey = actions[ska::DoAction];
	const auto backKey = actions[ska::Cancel] || actions[ska::DeleteChar];
	const auto menuKey = actions[ska::Menu];
	const auto confirmed = confirmKey && !m_lastConfirm;
	const auto backed = backKey && !m_lastBack;
	const auto menu = menuKey && !m_lastMenu;
	const auto dx = (pressed(ska::MoveRight) ? 1 : 0) - (pressed(ska::MoveLeft) ? 1 : 0);
	const auto dy = (pressed(ska::MoveDown) ? 1 : 0) - (pressed(ska::MoveUp) ? 1 : 0);

	// A choice asked by a script comes first (on the map, in a battle)
	auto& prompt = m_worldState.prompt();
	if (prompt.active && m_promptShown && !prompt.answer.has_value()) {
		if (m_promptChoice.keys(event.icm)) {
			prompt.answer = static_cast<int>(m_promptChoice.selected());
		} else if (backed) {
			prompt.answer = -1;
		}
	} else if (m_mode == Mode::Closed) {
		if (menu) {
			open(Mode::Tabs);
		}
	} else if (menu) {
		close();
	} else if (backed) {
		back();
	} else if (confirmed) {
		confirm();
	} else if (dx != 0 || dy != 0) {
		move(dx, dy);
	}
	m_lastToggles = toggles;
	m_lastConfirm = confirmKey;
	m_lastBack = backKey;
	m_lastMenu = menuKey;
	return true;
}

bool TrainerMenu::onMouse(ska::InputMouseEvent& event) {
	const auto& position = event.icm.getRanges()[ska::MousePos];
	const auto click = event.icm.getActions()[ska::LClic];
	const auto clicked = click && !m_lastClick;
	m_lastClick = click;
	if (position.x == 0 && position.y == 0) {
		return true;
	}
	const auto at = ska::Point<int>{ static_cast<int>(position.x), static_cast<int>(position.y) };
	auto& prompt = m_worldState.prompt();
	for (const auto& hit : m_hits) {
		if (!Inside(hit.box, at)) {
			continue;
		}
		// The choice of a script
		if (prompt.active && hit.mode == Mode::Closed && hit.index >= 100) {
			const auto choice = static_cast<std::size_t>(hit.index - 100);
			m_promptChoice.select(choice);
			if (clicked && !prompt.answer.has_value()) {
				prompt.answer = static_cast<int>(choice);
			}
			return true;
		}
		if (prompt.active) {
			continue;
		}
		switch (hit.mode) {
		case Mode::Closed:
			// A card at the bottom : it opens its page
			if (clicked) {
				open(hit.index == 0 ? Mode::Team : (hit.index == 1 ? Mode::Bag : Mode::Options));
			}
			break;
		case Mode::Tabs:
			m_tab = hit.index;
			if (clicked) {
				m_mode = Mode::Tabs;
				confirm();
			}
			break;
		case Mode::Team:
		case Mode::Moving:
			if (m_mode == Mode::Team || m_mode == Mode::Moving || m_mode == Mode::Tabs) {
				m_teamCursor = hit.index;
				if (clicked) {
					if (m_mode == Mode::Tabs) {
						m_mode = Mode::Team;
					}
					confirm();
				}
			}
			break;
		case Mode::TeamActions:
			if (m_mode == Mode::TeamActions) {
				m_actionCursor = hit.index;
				if (clicked) {
					confirm();
				}
			}
			break;
		case Mode::Bag:
			if (m_mode == Mode::Bag || m_mode == Mode::Tabs) {
				m_bagCursor = hit.index;
				if (clicked) {
					m_mode = Mode::Bag;
					confirm();
				}
			}
			break;
		case Mode::BagTarget:
			if (m_mode == Mode::BagTarget) {
				m_targetCursor = hit.index;
				if (clicked) {
					confirm();
				}
			}
			break;
		case Mode::Options:
			if (m_mode == Mode::Options || m_mode == Mode::Tabs) {
				m_mode = Mode::Options;
				const auto row = hit.index % 10;
				m_optionCursor = row;
				if (clicked) {
					if (hit.index >= 10 && row < 3) {
						// On a slider : the volume where it is clicked
						const auto value = std::clamp(static_cast<float>(at.x - hit.box.x) / static_cast<float>(std::max(1, hit.box.w)), 0.F, 1.F);
						if (row == 0) {
							m_settings.setMasterVolume(value);
						} else if (row == 1) {
							m_settings.setMusicVolume(value);
						} else {
							m_settings.setSoundVolume(value);
						}
					} else {
						option(row, 1);
					}
				}
			}
			break;
		case Mode::Summary:
			if (clicked) {
				back();
			}
			break;
		}
		return true;
	}
	return true;
}

void TrainerMenu::move(int dx, int dy) {
	const auto count = static_cast<int>(m_team.size());
	switch (m_mode) {
	case Mode::Tabs:
		if (dy != 0) {
			m_tab = (m_tab + dy + Tabs) % Tabs;
		} else if (dx > 0 && m_tab < 3) {
			confirm();
		}
		break;
	case Mode::Team:
	case Mode::Moving:
		if (count == 0) {
			break;
		}
		if (dx < 0 && m_teamCursor % 2 == 0 && m_mode == Mode::Team) {
			m_mode = Mode::Tabs;
			break;
		}
		m_teamCursor = std::clamp(m_teamCursor + dx + 2 * dy, 0, count - 1);
		break;
	case Mode::TeamActions:
		m_actionCursor = (m_actionCursor + dy + Actions) % Actions;
		break;
	case Mode::Summary:
		if (count > 0) {
			m_teamCursor = (m_teamCursor + dx + dy + count) % count;
		}
		break;
	case Mode::Bag:
		if (dx < 0) {
			m_mode = Mode::Tabs;
		} else if (!m_items.empty()) {
			m_bagCursor = (m_bagCursor + dy + static_cast<int>(m_items.size())) % static_cast<int>(m_items.size());
		}
		break;
	case Mode::BagTarget:
		if (count > 0) {
			m_targetCursor = (m_targetCursor + dy + count) % count;
		}
		break;
	case Mode::Options:
		if (dy != 0) {
			m_optionCursor = (m_optionCursor + dy + Options) % Options;
		} else {
			option(m_optionCursor, dx);
		}
		break;
	default:
		break;
	}
}

void TrainerMenu::option(int row, int direction) {
	const auto step = 0.05F * static_cast<float>(direction);
	switch (row) {
	case 0:
		m_settings.setMasterVolume(std::clamp(m_settings.getMasterVolume() / 100.F + step, 0.F, 1.F));
		break;
	case 1:
		m_settings.setMusicVolume(std::clamp(m_settings.getMusicVolume() / 100.F + step, 0.F, 1.F));
		break;
	case 2:
		m_settings.setSoundVolume(std::clamp(m_settings.getSoundVolume() / 100.F + step, 0.F, 1.F));
		break;
	case 3:
		m_settings.setWeather(!m_settings.getWeatherActive());
		break;
	case 4:
		m_settings.setFog(!m_settings.getFogActive());
		break;
	default:
		break;
	}
}

void TrainerMenu::confirm() {
	switch (m_mode) {
	case Mode::Tabs:
		if (m_tab == 3) {
			close();
		} else {
			m_mode = m_tab == 0 ? Mode::Team : (m_tab == 1 ? Mode::Bag : Mode::Options);
		}
		break;
	case Mode::Team:
		if (!m_team.empty()) {
			m_mode = Mode::TeamActions;
			m_actionCursor = 0;
		}
		break;
	case Mode::TeamActions:
		if (m_actionCursor == 0) {
			m_mode = Mode::Summary;
		} else if (m_actionCursor == 1 && m_team.size() > 1) {
			m_mode = Mode::Moving;
			m_movingFrom = m_teamCursor;
		} else {
			m_mode = Mode::Team;
		}
		break;
	case Mode::Moving:
		if (m_movingFrom >= 0 && m_movingFrom != m_teamCursor) {
			const auto name = m_team[static_cast<std::size_t>(m_movingFrom)].name;
			Team{ m_worldState.getSaveGame().getPathName() }.move(m_movingFrom, m_teamCursor);
			SLOG(ska::LogLevel::Info) << "Team : the Pokemon " << m_movingFrom << " goes to the place " << m_teamCursor;
			reload();
			notice(m_teamCursor == 0 ? name + " passe en tête de l'équipe !" : name + " change de place.");
		}
		m_movingFrom = -1;
		m_mode = Mode::Team;
		break;
	case Mode::Summary:
		m_mode = Mode::Team;
		break;
	case Mode::Bag:
		if (m_items.empty()) {
			break;
		}
		if (m_items[static_cast<std::size_t>(m_bagCursor)].heal) {
			m_mode = Mode::BagTarget;
			m_targetCursor = 0;
		} else if (m_items[static_cast<std::size_t>(m_bagCursor)].capture) {
			notice("Elle se lance pendant un combat.");
		} else {
			notice("Ce n'est pas le moment de l'utiliser.");
		}
		break;
	case Mode::BagTarget:
		useItem(m_targetCursor);
		break;
	case Mode::Options:
		option(m_optionCursor, 1);
		break;
	default:
		break;
	}
}

void TrainerMenu::back() {
	switch (m_mode) {
	case Mode::Tabs:
		close();
		break;
	case Mode::Team:
	case Mode::Bag:
	case Mode::Options:
		m_mode = Mode::Tabs;
		break;
	case Mode::TeamActions:
	case Mode::Summary:
	case Mode::Moving:
		m_movingFrom = -1;
		m_mode = Mode::Team;
		break;
	case Mode::BagTarget:
		m_mode = Mode::Bag;
		break;
	default:
		break;
	}
}

void TrainerMenu::useItem(int teamIndex) {
	if (m_items.empty() || teamIndex < 0 || teamIndex >= static_cast<int>(m_team.size())) {
		return;
	}
	const auto& item = m_items[static_cast<std::size_t>(m_bagCursor)];
	const auto& member = m_team[static_cast<std::size_t>(teamIndex)];
	const auto before = member.hp;
	const auto name = member.name;
	const auto itemName = item.name;
	if (UseItem(m_worldState, m_worldState.getBag(), item.id, member.index)) {
		reload();
		const auto after = teamIndex < static_cast<int>(m_team.size()) ? m_team[static_cast<std::size_t>(teamIndex)].hp : before;
		notice(name + " récupère " + std::to_string(std::max(0, after - before)) + " PV !");
		if (m_items.empty() || m_bagCursor >= static_cast<int>(m_items.size()) || m_items[static_cast<std::size_t>(m_bagCursor)].id != item.id || m_items[static_cast<std::size_t>(m_bagCursor)].amount <= 0) {
			m_mode = Mode::Bag;
		}
	} else {
		notice(itemName + " n'a aucun effet sur " + name + ".");
	}
}

const ska::Texture& TrainerMenu::charset(int id) const {
	auto found = m_charsets.find(id);
	if (found == m_charsets.end()) {
		found = m_charsets.emplace(id, ska::Texture{ ska::SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, id) }).first;
	}
	return found->second;
}

void TrainerMenu::pokemon(Ui::Canvas& canvas, int id, int x, int y, float scale) const {
	if (id < 0) {
		return;
	}
	const auto& texture = charset(id);
	const auto frame = static_cast<int>(texture.getHeight()) / 8;
	if (frame <= 0) {
		return;
	}
	// From the front, its walk
	const auto column = static_cast<int>(m_time / 260) % 3;
	const auto clip = ska::Rectangle{ column * frame, 2 * frame, frame, frame };
	canvas.image(texture, x - frame / 2, y - frame, &clip, scale, 255, true);
}

void TrainerMenu::hint(Ui::Canvas& canvas, const std::string& text) const {
	const auto size = screen();
	canvas.text(text, Style(15, White, 2), size.x / 2, size.y - 30, Ui::Align::Center);
}

void TrainerMenu::render(Ui::Canvas& canvas) const {
	m_hits.clear();
	const auto size = screen();
	if (m_mode == Mode::Closed) {
		if (available()) {
			renderCards(canvas);
		}
	} else {
		const auto in = std::min(1.F, static_cast<float>(m_openTime) / 160.F);
		canvas.fill(ska::Color{ 10, 14, 30, static_cast<unsigned char>(130.F * in) }, ska::Rectangle{ 0, 0, size.x, size.y });
		const auto slide = static_cast<int>((1.F - in) * 40.F);
		const auto tabs = ska::Rectangle{ 32 - slide, 72, 230, size.y - 150 };
		const auto content = ska::Rectangle{ 280 + slide, 72, size.x - 312, size.y - 150 };
		// The header : the trainer, its money
		canvas.text("Menu du dresseur", Style(30, White, 3), 36, 24, Ui::Align::Left);
		canvas.text("Argent : " + std::to_string(m_money), Style(22, Gold, 3), size.x - 36, 28, Ui::Align::Right);
		renderTabs(canvas, tabs);
		canvas.panel("panel_small.png", content, 16, 245);
		switch (m_mode) {
		case Mode::Tabs:
			if (m_tab == 0) {
				renderTeam(canvas, content);
			} else if (m_tab == 1) {
				renderBag(canvas, content);
			} else if (m_tab == 2) {
				renderOptions(canvas, content);
			} else {
				canvas.text("À bientôt !", Style(26, Soft), content.x + content.w / 2, content.y + content.h / 2 - 16, Ui::Align::Center);
			}
			hint(canvas, "W S : choisir    Entrée / D : ouvrir    Échap : fermer");
			break;
		case Mode::Team:
		case Mode::TeamActions:
		case Mode::Moving:
			renderTeam(canvas, content);
			break;
		case Mode::Summary:
			renderSummary(canvas, content);
			hint(canvas, "A D : autre Pokémon    Échap : retour");
			break;
		case Mode::Bag:
		case Mode::BagTarget:
			renderBag(canvas, content);
			break;
		case Mode::Options:
			renderOptions(canvas, content);
			hint(canvas, "W S : choisir    A D : régler    Échap : retour");
			break;
		default:
			break;
		}
		// A message (a Pokemon moved, an item used)
		if (!m_notice.empty() && m_noticeAge < 2600) {
			const auto alpha = 255.F * std::min(1.F, static_cast<float>(2600 - m_noticeAge) / 400.F);
			const auto box = ska::Rectangle{ content.x + content.w / 2 - 260, content.y + content.h - 64, 520, 46 };
			canvas.panel("panel_small_teal.png", box, 12, static_cast<unsigned char>(alpha));
			canvas.text(m_notice, Style(18, White, 0, alpha), box.x + box.w / 2, box.y + 12, Ui::Align::Center);
		}
	}
	renderPrompt(canvas);
}

void TrainerMenu::renderCards(Ui::Canvas& canvas) const {
	const auto size = screen();
	constexpr int Card = 76;
	constexpr int Gap = 10;
	const auto x0 = size.x - 3 * Card - 2 * Gap - 16;
	const auto y = size.y - Card - 16;
	const char* labels[] = { "Équipe", "Sac", "Options" };
	canvas.text("Tab : menu", Style(13, White, 2), x0 + (3 * Card + 2 * Gap) / 2, y - 22, Ui::Align::Center);
	for (auto i = 0; i < 3; i++) {
		const auto box = ska::Rectangle{ x0 + i * (Card + Gap), y, Card, Card };
		canvas.panel("panel_small.png", box, 12, 235);
		if (i == 0) {
			pokemon(canvas, m_leadId, box.x + Card / 2, box.y + Card - 14, 0.75F);
		} else if (i == 1) {
			const auto frame = static_cast<int>(m_ball.getHeight());
			const auto clip = ska::Rectangle{ 0, 0, frame, frame };
			canvas.image(m_ball, box.x + Card / 2 - frame / 2, box.y + 18, &clip, 2.4F);
		} else {
			// The options : three sliders
			for (auto k = 0; k < 3; k++) {
				const auto ly = box.y + 16 + k * 12;
				canvas.fill(Soft, ska::Rectangle{ box.x + 18, ly, Card - 36, 3 });
				const auto knob = box.x + 18 + (Card - 44) * ((k * 37 + 20) % 100) / 100;
				canvas.fill(Ui::Teal, ska::Rectangle{ knob, ly - 3, 8, 9 });
			}
		}
		canvas.text(labels[i], Style(13, Ink), box.x + Card / 2, box.y + Card - 22, Ui::Align::Center);
		m_hits.push_back(Hit{ box, Mode::Closed, i });
	}
}

void TrainerMenu::renderTabs(Ui::Canvas& canvas, const ska::Rectangle& area) const {
	const auto focused = m_mode == Mode::Tabs;
	for (auto i = 0; i < Tabs; i++) {
		const auto box = ska::Rectangle{ area.x, area.y + i * 74, area.w, 64 };
		const auto selected = i == m_tab;
		canvas.panel(selected ? (focused ? "panel_small_teal.png" : "panel_small_dark.png") : "panel_small.png", box, 14, 240);
		const auto ink = selected ? White : Ink;
		canvas.text(TabNames[i], Style(22, ink), box.x + 20, box.y + 8, Ui::Align::Left);
		canvas.text(TabDetails[i], Style(13, selected ? ska::Color{ 220, 240, 240, 255 } : Soft), box.x + 20, box.y + 38, Ui::Align::Left);
		if (selected && focused) {
			const auto bob = static_cast<int>(3.F * std::sin(static_cast<float>(m_time) / 120.F));
			canvas.text(">", Style(24, White), box.x + box.w - 28 + bob, box.y + 16, Ui::Align::Left);
		}
		m_hits.push_back(Hit{ box, Mode::Tabs, i });
	}
}

void TrainerMenu::renderTeam(Ui::Canvas& canvas, const ska::Rectangle& area) const {
	const auto focused = m_mode == Mode::Team || m_mode == Mode::TeamActions || m_mode == Mode::Moving;
	if (m_team.empty()) {
		canvas.text("Aucun Pokémon dans l'équipe.", Style(20, Soft), area.x + area.w / 2, area.y + 40, Ui::Align::Center);
		return;
	}
	const auto pad = 18;
	const auto cardW = (area.w - 3 * pad) / 2;
	const auto cardH = (area.h - 4 * pad - 40) / 3;
	for (auto i = 0; i < static_cast<int>(m_team.size()); i++) {
		const auto& member = m_team[static_cast<std::size_t>(i)];
		const auto box = ska::Rectangle{ area.x + pad + (i % 2) * (cardW + pad), area.y + pad + (i / 2) * (cardH + pad), cardW, cardH };
		const auto selected = focused && i == m_teamCursor;
		const auto moving = m_mode == Mode::Moving && i == m_movingFrom;
		const auto knocked = member.hp <= 0;
		canvas.panel(selected ? "panel_small_teal.png" : (knocked ? "panel_small_red.png" : "panel_small_dark.png"), box, 14, moving ? 140 : 240);
		pokemon(canvas, member.id, box.x + 54, box.y + cardH + 6, 1.7F);
		const auto textX = box.x + 110;
		canvas.text(member.name, Style(20, White), textX, box.y + 10, Ui::Align::Left);
		canvas.text("N. " + std::to_string(member.level), Style(16, Gold), box.x + box.w - 14, box.y + 12, Ui::Align::Right);
		auto bar = Ui::Bar{};
		bar.ratio = static_cast<float>(member.hp) / static_cast<float>(member.maxHp);
		bar.trail = bar.ratio;
		bar.fill = bar.ratio > 0.5F ? ska::Color{ 90, 210, 110, 255 } : (bar.ratio > 0.2F ? ska::Color{ 240, 200, 60, 255 } : ska::Color{ 230, 70, 60, 255 });
		canvas.bar(ska::Rectangle{ textX, box.y + 42, box.w - 110 - 18, 10 }, bar);
		canvas.text(knocked ? "K.O." : std::to_string(member.hp) + " / " + std::to_string(member.maxHp) + " PV", Style(13, White), textX, box.y + 58, Ui::Align::Left);
		auto px = textX;
		px += TypePill(canvas, member.type1, px, box.y + cardH - 30);
		TypePill(canvas, member.type2, px, box.y + cardH - 30);
		if (i == 0) {
			canvas.text("EN TÊTE", Style(11, Gold), box.x + box.w - 14, box.y + cardH - 24, Ui::Align::Right);
		}
		if (moving) {
			canvas.text("À DÉPLACER", Style(13, Gold, 2), box.x + box.w / 2, box.y + cardH / 2 - 8, Ui::Align::Center);
		}
		m_hits.push_back(Hit{ box, m_mode == Mode::Moving ? Mode::Moving : Mode::Team, i });
	}
	// The actions of the Pokemon chosen
	if (m_mode == Mode::TeamActions && m_teamCursor < static_cast<int>(m_team.size())) {
		const auto i = m_teamCursor;
		const auto cardX = area.x + pad + (i % 2) * (cardW + pad);
		const auto cardY = area.y + pad + (i / 2) * (cardH + pad);
		const auto menu = ska::Rectangle{ std::min(cardX + cardW - 60, area.x + area.w - 200), std::min(cardY + 20, area.y + area.h - 170), 190, 150 };
		canvas.panel("panel_small.png", menu, 14, 250);
		for (auto k = 0; k < Actions; k++) {
			const auto row = ska::Rectangle{ menu.x + 10, menu.y + 12 + k * 44, menu.w - 20, 38 };
			const auto selected = k == m_actionCursor;
			if (selected) {
				canvas.panel("panel_small_teal.png", row, 10, 255);
			}
			const auto disabled = k == 1 && m_team.size() < 2;
			canvas.text(ActionNames[k], Style(18, selected ? White : (disabled ? Soft : Ink)), row.x + 16, row.y + 8, Ui::Align::Left);
			m_hits.push_back(Hit{ row, Mode::TeamActions, k });
		}
	}
	if (m_mode == Mode::Moving) {
		hint(canvas, "W S A D : la nouvelle place    Entrée : poser    Échap : annuler");
	} else if (m_mode == Mode::TeamActions) {
		hint(canvas, "W S : choisir    Entrée : valider    Échap : retour");
	} else if (m_mode == Mode::Team) {
		hint(canvas, "W S A D : choisir    Entrée : actions    Échap : retour");
	}
}

void TrainerMenu::renderSummary(Ui::Canvas& canvas, const ska::Rectangle& area) const {
	if (m_teamCursor >= static_cast<int>(m_team.size())) {
		return;
	}
	const auto& member = m_team[static_cast<std::size_t>(m_teamCursor)];
	// Its portrait, big, on a lit floor
	const auto left = ska::Rectangle{ area.x + 20, area.y + 20, area.w * 2 / 5, area.h - 40 };
	canvas.panel("panel_small_dark.png", left, 16, 240);
	canvas.fill(ska::Color{ 255, 255, 255, 30 }, ska::Rectangle{ left.x + 30, left.y + left.h - 90, left.w - 60, 24 });
	pokemon(canvas, member.id, left.x + left.w / 2, left.y + left.h - 40, 4.2F);
	canvas.text(member.name, Style(28, White), left.x + left.w / 2, left.y + 16, Ui::Align::Center);
	canvas.text("N. " + std::to_string(member.level), Style(18, Gold), left.x + left.w / 2, left.y + 54, Ui::Align::Center);
	m_hits.push_back(Hit{ left, Mode::Summary, 0 });

	auto x = left.x + left.w + 24;
	auto y = area.y + 26;
	const auto width = area.x + area.w - 24 - x;
	auto px = x;
	px += TypePill(canvas, member.type1, px, y);
	TypePill(canvas, member.type2, px, y);
	y += 40;
	canvas.text("Points de vie", Style(15, Soft), x, y, Ui::Align::Left);
	canvas.text(std::to_string(member.hp) + " / " + std::to_string(member.maxHp), Style(15, Ink), x + width, y, Ui::Align::Right);
	auto bar = Ui::Bar{};
	bar.ratio = static_cast<float>(member.hp) / static_cast<float>(member.maxHp);
	bar.trail = bar.ratio;
	bar.fill = bar.ratio > 0.5F ? ska::Color{ 90, 210, 110, 255 } : (bar.ratio > 0.2F ? ska::Color{ 240, 200, 60, 255 } : ska::Color{ 230, 70, 60, 255 });
	canvas.bar(ska::Rectangle{ x, y + 24, width, 12 }, bar);
	y += 52;
	canvas.text("Expérience", Style(15, Soft), x, y, Ui::Align::Left);
	auto xp = Ui::Bar{};
	xp.ratio = member.experience;
	xp.trail = xp.ratio;
	xp.fill = ska::Color{ 80, 170, 240, 255 };
	canvas.bar(ska::Rectangle{ x, y + 24, width, 8 }, xp);
	y += 54;
	canvas.text("Capacités", Style(20, Ink), x, y, Ui::Align::Left);
	y += 34;
	for (auto k = std::size_t{ 0 }; k < member.skills.size(); k++) {
		const auto row = ska::Rectangle{ x, y + static_cast<int>(k) * 50, width, 42 };
		const auto type = k < member.skillTypes.size() ? member.skillTypes[k] : std::string{};
		canvas.fill(TypeColor(type), row);
		canvas.fill(ska::Color{ 255, 255, 255, 50 }, ska::Rectangle{ row.x, row.y, row.w, 8 });
		canvas.text(member.skills[k], Style(18, White, 2), row.x + 14, row.y + 10, Ui::Align::Left);
		canvas.text(type, Style(13, White, 2), row.x + row.w - 12, row.y + 13, Ui::Align::Right);
	}
	if (member.skills.empty()) {
		canvas.text("Aucune capacité connue.", Style(16, Soft), x, y, Ui::Align::Left);
	}
}

void TrainerMenu::renderBag(Ui::Canvas& canvas, const ska::Rectangle& area) const {
	const auto focused = m_mode == Mode::Bag || m_mode == Mode::BagTarget;
	const auto listW = area.w * 11 / 20;
	if (m_items.empty()) {
		canvas.text("Le sac est vide.", Style(20, Soft), area.x + listW / 2, area.y + 40, Ui::Align::Center);
	}
	for (auto i = 0; i < static_cast<int>(m_items.size()); i++) {
		const auto& item = m_items[static_cast<std::size_t>(i)];
		const auto row = ska::Rectangle{ area.x + 18, area.y + 18 + i * 52, listW - 18, 44 };
		const auto selected = focused && i == m_bagCursor;
		canvas.panel(selected ? "panel_small_teal.png" : "panel_small_dark.png", row, 12, 240);
		canvas.text(item.name, Style(18, White), row.x + 18, row.y + 10, Ui::Align::Left);
		canvas.text("x " + std::to_string(item.amount), Style(16, Gold), row.x + row.w - 16, row.y + 12, Ui::Align::Right);
		m_hits.push_back(Hit{ row, Mode::Bag, i });
	}
	// The item chosen : its description
	const auto detail = ska::Rectangle{ area.x + listW + 14, area.y + 18, area.w - listW - 32, area.h - 36 };
	if (!m_items.empty() && m_bagCursor < static_cast<int>(m_items.size())) {
		const auto& item = m_items[static_cast<std::size_t>(m_bagCursor)];
		canvas.text(item.name, Style(24, Ink), detail.x + 8, detail.y + 4, Ui::Align::Left);
		auto y = detail.y + 44;
		for (const auto& line : Ui::Wrap(item.description, static_cast<std::size_t>(std::max(12, detail.w / 10)))) {
			canvas.text(line, Style(16, Soft), detail.x + 8, y, Ui::Align::Left);
			y += 24;
		}
		y += 12;
		canvas.text(item.heal ? "Soigne un Pokémon de l'équipe." : (item.capture ? "Se lance sur un Pokémon sauvage, en combat." : ""), Style(15, Ink), detail.x + 8, y, Ui::Align::Left);
	}
	// On which Pokemon
	if (m_mode == Mode::BagTarget) {
		const auto box = ska::Rectangle{ detail.x, detail.y + detail.h / 3, detail.w, std::min(detail.h * 2 / 3, 40 + static_cast<int>(m_team.size()) * 50) };
		canvas.panel("panel_small.png", box, 14, 252);
		canvas.text("Sur quel Pokémon ?", Style(17, Ink), box.x + 16, box.y + 10, Ui::Align::Left);
		for (auto i = 0; i < static_cast<int>(m_team.size()); i++) {
			const auto& member = m_team[static_cast<std::size_t>(i)];
			const auto row = ska::Rectangle{ box.x + 10, box.y + 38 + i * 48, box.w - 20, 42 };
			const auto selected = i == m_targetCursor;
			if (selected) {
				canvas.panel("panel_small_teal.png", row, 10, 255);
			}
			canvas.text(member.name, Style(16, selected ? White : Ink), row.x + 12, row.y + 4, Ui::Align::Left);
			auto bar = Ui::Bar{};
			bar.ratio = static_cast<float>(member.hp) / static_cast<float>(member.maxHp);
			bar.trail = bar.ratio;
			bar.fill = ska::Color{ 90, 210, 110, 255 };
			canvas.bar(ska::Rectangle{ row.x + 12, row.y + 28, row.w / 2, 6 }, bar);
			canvas.text(std::to_string(member.hp) + " / " + std::to_string(member.maxHp), Style(13, selected ? White : Soft), row.x + row.w - 10, row.y + 20, Ui::Align::Right);
			m_hits.push_back(Hit{ row, Mode::BagTarget, i });
		}
		hint(canvas, "W S : choisir    Entrée : utiliser    Échap : retour");
	} else if (m_mode == Mode::Bag) {
		hint(canvas, "W S : choisir    Entrée : utiliser    Échap : retour");
	}
}

void TrainerMenu::renderOptions(Ui::Canvas& canvas, const ska::Rectangle& area) const {
	const auto focused = m_mode == Mode::Options;
	const float volumes[] = { m_settings.getMasterVolume(), m_settings.getMusicVolume(), m_settings.getSoundVolume() };
	const bool toggles[] = { m_settings.getWeatherActive(), m_settings.getFogActive() };
	for (auto i = 0; i < Options; i++) {
		const auto row = ska::Rectangle{ area.x + 18, area.y + 18 + i * 64, area.w - 36, 54 };
		const auto selected = focused && i == m_optionCursor;
		canvas.panel(selected ? "panel_small_teal.png" : "panel_small_dark.png", row, 12, 240);
		canvas.text(OptionNames[i], Style(19, White), row.x + 18, row.y + 14, Ui::Align::Left);
		if (i < 3) {
			// A slider : the bar, its value
			const auto track = ska::Rectangle{ row.x + row.w / 2, row.y + 22, row.w / 2 - 90, 10 };
			auto bar = Ui::Bar{};
			bar.ratio = volumes[i] / 100.F;
			bar.trail = bar.ratio;
			bar.fill = Gold;
			canvas.bar(track, bar);
			canvas.text(std::to_string(static_cast<int>(std::lround(volumes[i]))) + " %", Style(17, White), row.x + row.w - 16, row.y + 15, Ui::Align::Right);
			m_hits.push_back(Hit{ ska::Rectangle{ track.x, row.y, track.w, row.h }, Mode::Options, 10 + i });
		} else {
			const auto on = toggles[i - 3];
			const auto pill = ska::Rectangle{ row.x + row.w - 110, row.y + 12, 92, 30 };
			canvas.fill(on ? ska::Color{ 90, 200, 120, 255 } : ska::Color{ 120, 120, 140, 255 }, pill);
			canvas.text(on ? "OUI" : "NON", Style(16, White), pill.x + pill.w / 2, pill.y + 6, Ui::Align::Center);
		}
		m_hits.push_back(Hit{ row, Mode::Options, i });
	}
}

void TrainerMenu::renderPrompt(Ui::Canvas& canvas) const {
	const auto& prompt = m_worldState.prompt();
	if (!prompt.active || !m_promptShown) {
		return;
	}
	const auto size = screen();
	const auto count = static_cast<int>(prompt.choices.size());
	const auto box = ska::Rectangle{ size.x / 2 - 260, size.y / 2 - (80 + count * 48) / 2, 520, 80 + count * 48 };
	canvas.fill(ska::Color{ 10, 14, 30, 120 }, ska::Rectangle{ 0, 0, size.x, size.y });
	canvas.panel("panel_small.png", box, 16, 252);
	canvas.text(prompt.question, Style(20, Ink), box.x + box.w / 2, box.y + 16, Ui::Align::Center);
	for (auto i = 0; i < count; i++) {
		const auto row = ska::Rectangle{ box.x + 20, box.y + 56 + i * 48, box.w - 40, 42 };
		const auto selected = static_cast<std::size_t>(i) == m_promptChoice.selected();
		canvas.panel(selected ? "panel_small_teal.png" : "panel_small_dark.png", row, 10, 245);
		canvas.text(prompt.choices[static_cast<std::size_t>(i)], Style(18, White), row.x + 18, row.y + 10, Ui::Align::Left);
		if (selected) {
			const auto bob = static_cast<int>(3.F * std::sin(static_cast<float>(m_time) / 120.F));
			canvas.text(">", Style(20, White), row.x + row.w - 28 + bob, row.y + 9, Ui::Align::Left);
		}
		m_hits.push_back(Hit{ row, Mode::Closed, 100 + i });
	}
	canvas.text("W S : choisir    Entrée : valider    Échap : annuler", Style(15, White, 2), size.x / 2, box.y + box.h + 14, Ui::Align::Center);
}
