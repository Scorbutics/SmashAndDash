#include "SlotPokemon.h"
#include "../../ska/Graphic/SpritePath.h"
#include "../../Gameplay/Fight/StatisticsChangeEvent.h"
#include "../../ska/Utils/StringUtils.h"
#include "../../Gameplay/Data/Statistics.h"
#include "../../Utils/IDs.h"


SlotPokemon::SlotPokemon(Widget& parent, ska::Point<int> relativePos) :
WindowIG<ska::ValueChangedEventListener<SlotPokemonDataPtr*>, ska::HoverEventListener, ska::ClickEventListener>(parent, ska::Rectangle{ relativePos.x, relativePos.y, 10 * TAILLEBLOCFENETRE, 2 * TAILLEBLOCFENETRE }, ska::Button::MENU_DEFAULT_THEME_PATH + "menu"),
	m_fontSize(11) {
	m_data = nullptr;
	
	Widget& tthis = *this;
	auto spriteArea = std::unique_ptr<ska::ButtonSprite>(new ska::ButtonSprite(tthis, ska::Point<int>(4, 4), "", "", ska::Rectangle{ 0 }, [&](Widget*, ska::ClickEvent&) {
	}));
	auto nameArea = std::unique_ptr<ska::Label>(new ska::Label(*this, "Nom : ", m_fontSize, ska::Point<int>(112, 4)));
	auto levelArea = std::unique_ptr<ska::Label>(new ska::Label(*this, "Niveau : ", m_fontSize, ska::Point<int>(112, 22)));
	auto type1Area = std::unique_ptr<ska::Label>(new ska::Label(*this, "Type 1 : ", m_fontSize, ska::Point<int>(112, 40)));
	auto pvArea = std::unique_ptr<ska::Label>(new ska::Label(*this, "PVs : ", m_fontSize, ska::Point<int>(208, 22)));
	auto type2Area = std::unique_ptr<ska::Label>(new ska::Label(*this, "Type 2 : ", m_fontSize, ska::Point<int>(208, 40)));

	m_sprite = addWidget(spriteArea);
	m_name = addWidget(nameArea);
	m_level = addWidget(levelArea);
	m_hp = addWidget(pvArea);
	m_type1 = addWidget(type1Area);
	m_type2 = addWidget(type2Area);

	addHandler<ska::ClickEventListener>([this](Widget* tthis, ska::ClickEvent& e) {
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







