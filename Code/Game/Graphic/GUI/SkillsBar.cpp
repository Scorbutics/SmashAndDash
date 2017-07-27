#include "SkillsBar.h"
#include "../../Gameplay/Fight/SkillDescriptor.h"
#include "GUI/GUI.h"
#include "GUI/Components/Concrete/ButtonSprite.h"

SkillsBar::SkillsBar(ska::Widget& parent, const std::array<SkillDescriptor, 4>& skills) :
ska::DynamicWindowIG<ska::TimeEventListener, ska::KeyEventListener>(parent, ska::Rectangle{ 2* TAILLEBLOCFENETRE, 0, 0, 0 }, ska::GUI::MENU_DEFAULT_THEME_PATH + "menu") {
	//TODO 1 widget pour représenter chaque skill
	if (!skills.empty()) {
		ska::Point<int> buf;
		buf.x = TAILLEBLOCFENETRE / 2;
		buf.y = TAILLEBLOCFENETRE / 4;

		for (const auto& s : skills) {
			auto& bSkill = addWidget<ska::ButtonSprite>(buf, "", s.id, [&](ska::Widget*, ska::ClickEvent&) {
			});
			buf.x += TAILLEBLOCFENETRE;
			m_skillsHolder.push_back(&bSkill);
		}

		Widget::setWidth(m_skillsHolder[0]->getBox().w * skills.size() + 2 * TAILLEBLOCFENETRE);
		Widget::setHeight(m_skillsHolder[0]->getBox().h + TAILLEBLOCFENETRE);
	}
}
