// #include <string>
// #include <vector>
// 
// #include "MouseCursor.h"
// #include "../../ska/Graphic/SpritePath.h"
// #include "../../Gameplay/Data/Statistics.h"
// #include "../../ska/Utils/StringUtils.h"
// #include "../../ska/Utils/TimeUtils.h"
// #include "../../Gameplay/Inventory/Object.h"
// #include "../../ska/Inputs/InputContextManager.h"
// #include "../../Utils/IDs.h"
// //#include "GUI.h"
// 
// MouseCursor::MouseCursor(const ska::InputContextManager& playerICM) :
// m_aniCursor(3, 3, false), m_playerICM(playerICM),
// m_hintBox("hintbox", "", "."FILE_SEPARATOR"Menu"FILE_SEPARATOR"hintboxmenu.png", { 0, 0, 3 * TAILLEBLOCFENETRE, 2 * TAILLEBLOCFENETRE }, 20),
// m_sprite("."FILE_SEPARATOR"Sprites"FILE_SEPARATOR"Icones"FILE_SEPARATOR"mousec.png") {
// 	m_hideC = m_hideH = true;
//     m_delay = 0;
//     m_time = 0;
// 	
//     m_stockObject = nullptr;
// 	ska::Rectangle buf = { 0, 0 };
//     buf.w = m_sprite.getWidth() / 3;
//     buf.h = m_sprite.getHeight();
//     m_aniCursor.setOffsetAndFrameSize(buf); //initialisation de l'animation
// }
// 
// ska::Animation& MouseCursor::getAnimation() {
//     return m_aniCursor;
// }
// 
// void MouseCursor::modifyHint(const std::string& hint) {
//     m_hintBox.modifyText(hint);
// }
// 
// void MouseCursor::hideCursor(bool x) {
//     m_hideC = x;
// }
// 
// void MouseCursor::hideHint(bool x) {
// 	m_hintBox.hide(x);
// 	m_hideH = x;
// }
// 
// void MouseCursor::setObjectAmount(unsigned int x) {
//     m_objectAmount = x;
// }
// 
// bool MouseCursor::isActiveCursor() {
//     return !m_hideC;
// }
// 
// DialogMenu& MouseCursor::getHintBox() {	
// 	return m_hintBox;
// }
// 
// bool MouseCursor::isActiveHint() const {
// 	return !m_hideH;
// }
// 
// void MouseCursor::setCursorPos(ska::Rectangle pos) {
//     m_cursorPos = pos;
// }
// 
// void MouseCursor::showCursorTime(unsigned int delay) {
// 	m_time = ska::TimeUtils::getTicks();
//     m_delay = delay;
//     m_hideC = false;
// }
// 
// bool MouseCursor::isVisible() const {
// 	return !m_hideC || !m_hideH;
// }
// 
// void MouseCursor::update() {
// 	m_hintBox.refresh();
// 	ska::Point<float> mousePos = m_playerICM.getRanges()[ska::InputRangeType::MousePos];
// 	mousePos.y -= m_hintBox.getBox().w / 2;
// 	m_hintBox.move(mousePos);
// 	
// 	m_aniCursor.getRectOfCurrentFrame();
// }
// 
// void MouseCursor::display() const {
// 	if (m_hideC && m_hideH) {
// 		return;
// 	}
// 
//     if(ska::TimeUtils::getTicks() - m_time < m_delay) {
// 		ska::Rectangle buf = m_aniCursor.getOffsetAndFrameSize();
// 		m_sprite.render(m_cursorPos.x, m_cursorPos.y, &buf);
//     }
// 
// }
// 
// void MouseCursor::displaySelectedPokemon() const {
// 	const ska::InputRange& mousePos = m_playerICM.getRanges()[ska::InputRangeType::MousePos];
// 	/*ska::Rectangle rectPkmnSprite;
//     if(m_stockPkmn != NULL)
//     {
// 		ska::Texture pkmn(ska::SpritePath::getInstance().getPath(SPRITEBANK_CHARSET, m_stockPkmn->getID()), DEFAULT_T_RED, DEFAULT_T_GREEN, DEFAULT_T_BLUE);
//         rectPkmnSprite.x = 0;
//         rectPkmnSprite.y = m_stockPkmn->getHeight();
//         rectPkmnSprite.w = m_stockPkmn->getWidth();
//         rectPkmnSprite.h = m_stockPkmn->getHeight();
// 		pkmn.render(mousePos.x, mousePos.y, &rectPkmnSprite);
//     }*/
// }
// 
// void MouseCursor::displaySelectedObject() const
// {
// 	const ska::InputRange& mousePos = m_playerICM.getRanges()[ska::InputRangeType::MousePos];
// 	ska::Point<float> pos = mousePos;
//     if(m_stockObject != nullptr) {
// 		pos.x += 10;
// 		m_stockObject->setPos(pos);
//         m_stockObject->display();
//     }
// 
// 
// }
// 
// void MouseCursor::displayHint() const {
// 	if (!m_hideH) {
// 		m_hintBox.display();
// 	}
// }
// 
// ObjectPtr& MouseCursor::getObject() {
// 	return m_stockObject;
// }
// 
// /*
// Character* MouseCursor::getPokemon()
// {
// 	if(m_stockPkmn != NULL)
// 		return &(*m_stockPkmn);
// 	else
// 		return NULL;
// }*/
// 
// 
// /*
// void MouseCursor::setPokemon(Character* pkmn)
// {
// 	m_stockPkmn = Character_ptr(new Character(pkmn->getID()));
// 	m_stockPkmn->setEntityNumber(pkmn->getEntityNumber());
// 	m_stockPkmn->setSpeedLimit(pkmn->getSpeedLimit());
// 	m_stockPkmn->setDirection(pkmn->getDirection());
// 	m_stockPkmn->setEntityNumber(pkmn->getEntityNumber());
// 	m_stockPkmn->teleport(pkmn->getPos().x, pkmn->getPos().y);
// 	m_stockPkmn->setHP(pkmn->getHp());
// 	m_stockPkmn->getStatistics()->setExperience(pkmn->getStatistics()->getExperience());
// 
// 	if(pkmn->getPath()->isMotionless())
// 		m_stockPkmn->getPath()->setPathString(ska::StringUtils::intToStr(P_FIXED));
// 	else if((pkmn->getPath()->isRandom()))
// 		m_stockPkmn->getPath()->setPathString(ska::StringUtils::intToStr(P_RANDOM));
// 	else
// 		m_stockPkmn->getPath()->setPathString(pkmn->getPath()->getPathString());
// 
// 	for(unsigned int j = 0; j < 4; j++)
// 		m_stockPkmn->setOffset(j, pkmn->getOffset(j));
// }
// 
// void MouseCursor::removePokemon()
// {
// 	Character_ptr buf = std::move(m_stockPkmn);
// 	m_stockPkmn = NULL;
// }
// */
// 
// void MouseCursor::removeObject() {
// 	m_stockObject = NULL;
// }
// 
// unsigned int MouseCursor::getObjectAmount() {
//     return m_objectAmount;
// }
// 
// 
// void MouseCursor::setObject(Object* object, unsigned int amount) {
// 	m_stockObject = std::unique_ptr<Object>(new Object(*object));
// 	m_objectAmount = amount;
// }
// 
// MouseCursor::~MouseCursor() {
// }
