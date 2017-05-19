#include <doctest.h>
#include "Point.h"
#include "WindowsUtil.h"
#include "GUI/Windows/MoveableWindow.h"

template<class ... T>
class MoveableWindowTest : public ska::MoveableWindow<T...> {
public:
	MoveableWindowTest(ska::Widget& parent, const ska::Rectangle& box, const std::string& styleName) : ska::MoveableWindow<T...>(parent, box, styleName) {
	}

	MoveableWindowTest(ska::MouseObservable& mo, ska::KeyObservable& ko, const ska::Rectangle& box, const std::string& styleName) : ska::MoveableWindow<T...>(mo, ko, box, styleName) {
	}

	virtual void display() const override {
		ska::MoveableWindow<T...>::display();
		m_callbackDisplay();
	}

	void whenDisplayed(std::function<void(void)> callback) {
		m_callbackDisplay = callback;
	}

private:
	std::function<void(void)> m_callbackDisplay;
};

class ObservableTest : public ska::MouseObservable, public ska::KeyObservable {

};


TEST_CASE("[MoveableWindow]Déplacement") {
	GetWindow();
	ObservableTest obs;
	MoveableWindowTest<> dw(obs, obs, ska::Rectangle{ 1, 1, 100, 20 }, "nostyle");

	//Clic sur la barre de titre pour "grab" la fenêtre
	ska::Point<int> cePoint(5, 2);
	ska::ClickEvent ce(ska::MouseEventType::MOUSE_CLICK, cePoint);
	obs.ska::ClickObservable::notifyObservers(ce);

	//Déplacement de la fenêtre selon l'axe horizontal positif (pixel par pixel)
	ska::Point<int> finalPosition(20, 1);
	ska::Point<int> heRelativePoint(5, 2);
	for(auto i = heRelativePoint.x; i < 20 + heRelativePoint.x; i++) {
		ska::Point<int> hePoint(i, 2);
		ska::HoverEvent he(ska::MouseEventType::MOUSE_OVER, hePoint, hePoint, heRelativePoint);
		obs.ska::HoverObservable::notifyObservers(he);
	}
	
	CHECK(dw.getBox().x == finalPosition.x);
	CHECK(dw.getBox().y == finalPosition.y);

	//TODO test avec MOUSE_RELEASE (vérifier cette fois que la fenêtre ne bouge plus)
}

TEST_CASE("[MoveableWindow]Clic sur le bouton de fermeture") {
	GetWindow();
	ObservableTest obs;
	MoveableWindowTest<> dw(obs, obs, ska::Rectangle{ 1, 1, 100, 20 }, "nostyle");

	CHECK(dw.isVisible());

	//Clic sur le bouton de fermeture (X)
	ska::Point<int> cePoint(90, 2);
	ska::ClickEvent ce(ska::MouseEventType::MOUSE_CLICK, cePoint);
	obs.ska::ClickObservable::notifyObservers(ce);

	CHECK(!dw.isVisible());
}
