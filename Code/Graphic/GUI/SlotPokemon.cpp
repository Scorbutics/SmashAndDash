#include "SlotPokemon.h"
#include "../../ska/Graphic/SpritePath.h"
#include "../../Gameplay/Fight/StatisticsChangeEvent.h"
#include "../../ska/Utils/StringUtils.h"
#include "../../Gameplay/Data/Statistics.h"
#include "../../Utils/IDs.h"


SlotPokemon::SlotPokemon(ska::Widget& parent, ska::Point<int> relativePos, unsigned int slot, unsigned int pokemonId) :
ska::WindowIG<ska::ValueChangedEventListener<SlotPokemonData>, ska::HoverEventListener, ska::ClickEventListener>(parent, ska::Rectangle{ relativePos.x, relativePos.y, 10 * TAILLEBLOCFENETRE, 2 * TAILLEBLOCFENETRE }, ska::Button::MENU_DEFAULT_THEME_PATH + "menu"),
	m_fontSize(11){
	m_data.id = pokemonId;
	m_data.slotNumber = slot;
	
	ska::Widget& tthis = *this;
	auto spriteArea = std::unique_ptr<ska::ButtonSprite>(new ska::ButtonSprite(tthis, ska::Point<int>(4, 4), "", ska::SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, m_data.id), ska::Rectangle{ 64, 64, 2 * 64, 2 * 64 }, [&](ska::Widget*, ska::ClickEvent& e) {
	}));
	auto nameArea = std::unique_ptr<ska::Label>(new ska::Label(*this, "Nom : ", m_fontSize, ska::Point<int>(112, 4)));
	auto levelArea = std::unique_ptr<ska::Label>(new ska::Label(*this, "Niveau : ", m_fontSize, ska::Point<int>(112, 22)));
	auto type1Area = std::unique_ptr<ska::Label>(new ska::Label(*this, "Type 1 : ", m_fontSize, ska::Point<int>(112, 40)));
	auto pvArea = std::unique_ptr<ska::Label>(new ska::Label(*this, "PVs : ", m_fontSize, ska::Point<int>(208, 22)));
	auto type2Area = std::unique_ptr<ska::Label>(new ska::Label(*this, "Type 2 : ", m_fontSize, ska::Point<int>(208, 40)));

	m_sprite = addWidget(spriteArea);
	addWidget(nameArea);
	addWidget(levelArea);
	addWidget(pvArea);
	addWidget(type1Area);
	addWidget(type2Area);

	addHandler<ska::ClickEventListener>([this](ska::Widget* tthis, ska::ClickEvent& e) {
		if (e.getState() == ska::MouseEventType::MOUSE_CLICK) {
			ska::ValueChangedEvent<SlotPokemonData> vce(m_data, m_data);
			directNotify(vce);
		}
	});

}

bool SlotPokemon::onStatisticsChangeEvent(StatisticsChangeEvent& sce) {
	return true;
}

void SlotPokemon::load(SlotPokemonData& spd) {
	m_data = spd;
	
	m_sprite->replaceWith(ska::SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, m_data.id), ska::Rectangle{ 64, 64, 2 * 64, 2 * 64 });
}

//
// void SlotPokemon_Area::setPokemon(unsigned int index) {
//     m_index = index;
//     //m_pkmn = wScreen.getPokemonManager().getPokemon(index);
// 	ska::Rectangle rectSrcBuf, buf;
//     buf.x = m_relativePos.x;
//     buf.y = m_relativePos.y;
//     rectSrcBuf.x = 0;
//     /*rectSrcBuf.y = m_pkmn->getHeight();
//     rectSrcBuf.w = m_pkmn->getWidth();
//     rectSrcBuf.h = m_pkmn->getHeight();
// 	m_spriteArea = unique_ptr<Image_Area>(new Image_Area(m_parent, buf, &rectSrcBuf, ska::SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, m_pkmn->getID()), false));
//     buf.x += rectSrcBuf.w + 4;
// 	buf.y += 6;
//     m_nameArea = unique_ptr<Text_Area>(new Text_Area(m_parent, "Nom : " + m_pkmn->getDescriptor()->getName(), 20, buf));
//     buf.x += 4*TAILLEBLOCFENETRE;
// 	m_levelArea = unique_ptr<Text_Area>(new Text_Area(m_parent, "Niveau : " + ska::StringUtils::intToStr(m_pkmn->getStatistics()->getLevel()), 20, buf));
//     buf.x -= 4*TAILLEBLOCFENETRE;
//     buf.y += 21;
//     m_type1Area = unique_ptr<Text_Area>(new Text_Area(m_parent, "Type 1 : " + m_pkmn->getDescriptor()->getType(1), 20, buf));
//     buf.x += 4*TAILLEBLOCFENETRE;
//     m_type2Area = unique_ptr<Text_Area>(new Text_Area(m_parent, "Type 2 : " + m_pkmn->getDescriptor()->getType(2), 20, buf));
//     buf.x -= 4*TAILLEBLOCFENETRE;
//     buf.y += 21;
// 	m_pvArea = unique_ptr<Text_Area>(new Text_Area(m_parent, "PVs : " + ska::StringUtils::uintToStr(m_pkmn->getHp()) + "/" + ska::StringUtils::uintToStr(m_pkmn->getStatistics()->getHpMax()), 20, buf));
//     if(m_action == "use_object")
//     {
//         vector<string> vUse;
//         vector<int> vBool;
//         vBool.push_back(0);
//         vBool.push_back(1);
//         vUse.push_back("Utiliser");
//         vUse.push_back("Ok !");
//         buf.x += 5*TAILLEBLOCFENETRE;
// 		m_buttonArea = unique_ptr<Button>(new Button(m_parent, buf, "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"button.png", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"buttonpressed.png", m_boolUseObjectSelectPkmn, vBool, vUse, 20, "team_use_object_" + ska::StringUtils::intToStr(m_key), false));
//         buf.x -= 5*TAILLEBLOCFENETRE;
//     }
//     else
//         m_buttonArea = NULL;
//
//     buf.x -= rectSrcBuf.w + 4;
//     buf.y -= 42;
//     buf.w = TAILLEBLOC;
//     buf.h = TAILLEBLOC;*/
//
// }

// void SlotPokemon_Area::display() const {
// 	ska::Point<int> buf = m_relativePos;
//     buf.x += m_parent.getRect().x;
// 	buf.y += m_parent.getRect().y;
//
//
//
// 	if (m_buttonArea != NULL) {
// 		m_buttonArea->display();
// 	}
//
// }





