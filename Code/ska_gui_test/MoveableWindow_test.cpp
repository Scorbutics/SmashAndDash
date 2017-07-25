#include <doctest.h>
#include "Point.h"
#include "WindowsUtil.h"
#include "MoveableWindowTest.h"

class ObservableTest : public ska::MouseObservable, public ska::KeyObservable {

};


TEST_CASE("[MoveableWindow]Déplacement") {
	//GetWindow();
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
	//GetWindow();
	ObservableTest obs;
	MoveableWindowTest<> dw(obs, obs, ska::Rectangle{ 1, 1, 100, 20 }, "nostyle");

	CHECK(dw.isVisible());

	//Clic sur le bouton de fermeture (X)
	ska::Point<int> cePoint(90, 2);
	ska::ClickEvent ce(ska::MouseEventType::MOUSE_CLICK, cePoint);
	obs.ska::ClickObservable::notifyObservers(ce);

	CHECK(!dw.isVisible());
}
