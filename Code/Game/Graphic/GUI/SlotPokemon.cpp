#include "SlotPokemon.h"
#include "Utils/SpritePath.h"
#include "../../Gameplay/Fight/StatisticsChangeEvent.h"
#include "../../Utils/IDs.h"


SlotPokemon::SlotPokemon(Widget& parent, ska::Point<int> relativePos) :
WindowIG<ska::ValueChangedEventListener<SlotPokemonDataPtr*>, ska::HoverEventListener, ska::ClickEventListener>(parent, ska::Rectangle{ relativePos.x, relativePos.y, 10 * TAILLEBLOCFENETRE, 2 * TAILLEBLOCFENETRE }, ska::Button::MENU_DEFAULT_THEME_PATH + "menu"),
	m_fontSize(11) {
	m_data = nullptr;

	Widget& tthis = *this;

	m_sprite = &addWidget<ska::ButtonSprite>(ska::Point<int>(4, 4), "", "", ska::Rectangle{ 0, 0, 0, 0 }, [&](Widget*, ska::ClickEvent&) {});
	m_name = &addWidget<ska::Label>("Nom : ", m_fontSize, ska::Point<int>(112, 4));
	m_level = &addWidget<ska::Label>("Niveau : ", m_fontSize, ska::Point<int>(112, 22));
	m_hp = &addWidget<ska::Label>("Type 1 : ", m_fontSize, ska::Point<int>(112, 40));
	m_type1 = &addWidget<ska::Label>("PVs : ", m_fontSize, ska::Point<int>(208, 22));
	m_type2 = &addWidget<ska::Label>("Type 2 : ", m_fontSize, ska::Point<int>(208, 40));

	addHandler<ska::ClickEventListener>([this](Widget*, ska::ClickEvent& e) {
		if (e.getState() == ska::MouseEventType::MOUSE_CLICK) {
			ska::ValueChangedEvent<SlotPokemonDataPtr*> vce(nullptr, &m_data);
			m_data->parent = this;
			directNotify(vce);
		}
	});

}

bool SlotPokemon::onStatisticsChangeEvent(StatisticsChangeEvent&) {
	return true;
}

void SlotPokemon::load(SlotPokemonDataPtr& spd) {
	m_data = std::move(spd);

	m_sprite->replaceWith(ska::SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, m_data->id), ska::Rectangle{ 1 * 64, 160, 64, 64 });
	m_name->modifyText("Nom : " + m_data->name);
	m_level->modifyText("Niveau : " + m_data->level);
	m_type1->modifyText("Type 1 : " + m_data->type1);
	m_hp->modifyText("PVs : " + m_data->hp);
	m_type2->modifyText(m_data->type2.empty() ? " " : ("Type 2 : " + m_data->type2));
}

void SlotPokemon::unload() {
	m_data = nullptr;
	show(false);
}

bool SlotPokemon::isLoaded() const {
	return m_data != nullptr;
}







