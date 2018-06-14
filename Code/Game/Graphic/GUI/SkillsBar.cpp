#include "SkillsBar.h"
#include "../../Gameplay/Fight/SkillDescriptor.h"
#include "GUI/GUI.h"
#include "GUI/Components/Concrete/ButtonSprite.h"

SkillsBar::SkillsBar(ska::Widget& parent, const std::array<SkillDescriptor, 4>& skills, unsigned int windowWidth, unsigned int windowHeight) :
ska::DynamicWindowIG<ska::TimeEventListener, ska::KeyEventListener>(parent, ska::Rectangle{ 64, static_cast<int>(windowHeight - 20 * 32), 0, 0 }) {
	//TODO 1 widget pour représenter chaque skill
	if (!skills.empty()) {
		ska::Point<int> buf;
		buf.x = 16;
		buf.y = 4;

		for (const auto& s : skills) {
			auto& bSkill = addWidget<ska::ButtonSprite>(buf, "", s.id, [&](ska::Widget*, ska::ClickEvent&) {
			});
			buf.x += 32;
			m_skillsHolder.push_back(&bSkill);
		}

		Widget::setWidth(m_skillsHolder[0]->getBox().w * skills.size() + 64);
		Widget::setHeight(m_skillsHolder[0]->getBox().h + 32);
	}
}
