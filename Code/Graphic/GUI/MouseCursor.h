// #pragma once
// 
// #include <string>
// #include <memory>
// #include "../../ska/Graphic/Draw/DrawableFixedPriority.h"
// #include "../../ska/Graphic/Animation.h"
// #include "../../ska/Graphic/Texture.h"
// #include "DialogMenu.h"
// 
// class GUI;
// 
// class Object;
// using ObjectPtr = std::unique_ptr<Object>;
// 
// 
// namespace ska {
// 	class InputContextManager;
// }
// 
// class MouseCursor : public ska::DrawableFixedPriority {
// public:
// 	MouseCursor(const ska::InputContextManager& playerICM);
//     ~MouseCursor();
// 
//     ska::Animation& getAnimation();
//     DialogMenu& getHintBox();
// 	ObjectPtr& getObject();
//     unsigned int getObjectAmount();
// 
//     void setObjectAmount(unsigned int x);
// 	void setObject(Object* object, unsigned int amount);
// 	void setCursorPos(ska::Rectangle pos);
// 
// 	void display() const override;
// 	bool isVisible() const override;
// 
// 	void update();
// 
// 	void removePokemon();
// 	void removeObject();
// 
// 	void modifyHint(const std::string& hint);
// 	void displaySelectedPokemon() const;
// 	void displaySelectedObject() const;
//     void displayHint() const;
//     void hideCursor(bool x);
//     void hideHint(bool x);
//     bool isActiveCursor();
//     bool isActiveHint() const;
//     void showCursorTime(unsigned int delay);
// 
// private:
// 	const ska::InputContextManager& m_playerICM;
//     ska::Animation m_aniCursor;
//     ska::Texture m_sprite;
// 	ska::Rectangle m_cursorPos;
//     DialogMenu m_hintBox;
//     bool m_hideC, m_hideH;
// 
//     ObjectPtr m_stockObject;
//     unsigned int m_objectAmount, m_time, m_delay;
// };
