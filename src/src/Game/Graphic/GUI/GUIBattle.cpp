#include "GUIBattle.h"
#include "Bar.h"
#include "GUI/GUI.h"
#include "GUI/Components/Concrete/Button.h"
#include "../../Gameplay/Fight/SkillsHolderComponent.h"
#include "../../Gameplay/Fight/BattleComponent.h"
#include "Draw/DrawableContainer.h"
#include "GUI/GUI.h"


GUIBattle::GUIBattle(PokemonGameEventDispatcher& ged) :
StatisticsChangeObserver(std::bind(&GUIBattle::onStatisticsChange, this, std::placeholders::_1)),
BattleStartObserver(std::bind(&GUIBattle::onBattleStart, this, std::placeholders::_1)),
//m_moves("", ska::Rectangle {0, w.getHeight() - 4 * TAILLEBLOCFENETRE, 9*TAILLEBLOCFENETRE, 2 * TAILLEBLOCFENETRE}, -1, false),
m_ged(ged),
m_skillsBar(nullptr) {
	//m_skillsBar = &addWindow<SkillsBar>("skillsBar", );

	m_ged.ska::Observable<BattleEvent>::addObserver(*this);
	m_ged.ska::Observable<StatisticsChangeEvent>::addObserver(*this);
}

bool GUIBattle::onStatisticsChange(StatisticsChangeEvent& sce) {
	const ska::EntityId& target = sce.getEntity();
	RawStatistics<int>& targetStats = sce.getStats();
	if (m_bars.find(target) != m_bars.end()) {
		m_bars[target]->setCurrentValue(targetStats.hp);
	}
	return true;
}



void GUIBattle::addHPBar(ska::CameraSystem& camSys, unsigned int maxValue, unsigned int currentValue, ska::EntityManager& em, const ska::EntityId& entityId) {
	m_bars[entityId] = std::move(std::make_unique<Bar>(camSys, ska::GUI::MENU_DEFAULT_THEME_PATH + "hpbar.png", ska::GUI::MENU_DEFAULT_THEME_PATH + "hpbarcontent.png", maxValue, em, entityId));
	m_bars[entityId]->setCurrentValue(currentValue);
}

bool GUIBattle::onBattleStart(BattleEvent& be) {
	if (be.getEventType() == BATTLE_START) {
		auto& camSys = be.getCameraSystem();
		const auto& pokemon = be.getBattler();
		const auto& opponent = be.getOpponent();
		auto& em = be.getEntityManager();

		const auto& pokemonBc = em.getComponent<BattleComponent>(pokemon);
		const auto& opponentBc = em.getComponent<BattleComponent>(opponent);

		//TODO max hp venant des stats du fichier ini
		addHPBar(camSys, pokemonBc.hp, pokemonBc.hp, em, pokemon);
		addHPBar(camSys, opponentBc.hp, opponentBc.hp, em, opponent);
	} else {
		removeHPBar(be.getBattler());
		removeHPBar(be.getOpponent());
	}
	return true;
}

/* TODO put the sprite in the GUI */
//m_icone.load("." FILE_SEPARATOR "Sprites" FILE_SEPARATOR "Icones" FILE_SEPARATOR + ska::StringUtils::intToStr(m_id) + ".png");

//TODO dans chacun des widgets de la SkillsBar
	//if (m_moves.isVisible()) {
	//	m_moves.refresh();

		//On blit les cooldowns par dessus
		/*for(unsigned int i = 0; i < v->size(); i++) {
		if(!(*v)[i]->cooldownOK()) {
		ska::Texture* cooldownText;
		cooldownText = (*v)[i]->getSpriteRemainingCD();
		cooldownPos.x = m_attackPokemon->getRect().x + TAILLEBLOC / 2 + (i + 1) * (*v)[i]->getIcon()->getWidth();
		cooldownText->render(cooldownPos.x, cooldownPos.y);
		}
		}*/

	//}


void GUIBattle::graphicUpdate(unsigned int, ska::DrawableContainer& drawables) {
	for (auto& b : m_bars) {
		drawables.add(*b.second);
	}
	//drawables.add(m_moves);
}

void GUIBattle::clear() {
	m_bars.clear();
}

GUIBattle::~GUIBattle() {
	m_ged.ska::Observable<StatisticsChangeEvent>::removeObserver(*this);
	m_ged.ska::Observable<BattleEvent>::removeObserver(*this);
	clear();
}

void GUIBattle::removeHPBar(const ska::EntityId& entity) {
	auto it = m_bars.find(entity);
	if (it != m_bars.end()) {
		m_bars.erase(it);
	}
}
