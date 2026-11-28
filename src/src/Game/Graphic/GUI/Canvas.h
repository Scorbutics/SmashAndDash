#pragma once
#include <functional>
#include <string>
#include <vector>
#include "Core/Draw/DrawableFixedPriority.h"
#include "Core/Rectangle.h"
#include "Core/Point.h"
#include "Core/Inputs/InputToggle.h"
#include "Core/Draw/Color.h"

namespace ska {
	class Renderer;
	class Texture;
	class InputContextManager;
}

// The screens that draw themselves each frame (the title, the battle) : immediate drawing with the theme of the
// interface (Theme : the images of Menu/default_theme), no widget tree. What changes every frame (bars, shakes, banners
// that scale and fade, marks that follow the fighters, the render effects) is drawn from the state as it is now.
// The menu of the trainer (TrainerMenu : the team, the bag, the options), the dialogs (DialogBox) and the debug panel
// (DebugPanel) too : the game uses no ska_gui window.
namespace Ui {
	// The colours of the theme : the ink of the tiles on the light panels, white on the coloured pills
	const ska::Color Ink{ 34, 40, 54, 255 };
	const ska::Color White{ 255, 255, 255, 255 };
	const ska::Color Faded{ 130, 134, 144, 255 };
	const ska::Color Teal{ 72, 190, 170, 255 };

	enum class Align { Left, Center, Right };

	struct TextStyle {
		unsigned int size = 16;
		ska::Color color = Ink;
		// An outline (readable over the map) : its thickness in pixels (0 : none) and colour
		int outline = 0;
		ska::Color outlineColor{ 0, 0, 0, 255 };
		float scale = 1.F;
		unsigned char alpha = 255;
	};

	// A bar of the theme in its dark groove : the part kept ("ratio"), the part just lost ("trail", >= ratio : it stays
	// a moment), filled from the left or from the right
	struct Bar {
		float ratio = 0.F;
		float trail = 0.F;
		ska::Color fill = Teal;
		ska::Color trailColor{ 255, 236, 200, 255 };
		bool fromRight = false;
	};

	class Canvas {
	public:
		explicit Canvas(ska::Renderer& renderer) : m_renderer(renderer) {}

		ska::Renderer& renderer() const { return m_renderer; }
		// A rectangle of a colour (blended when it is not opaque)
		void fill(const ska::Color& color, const ska::Rectangle& box) const;
		// A text (UTF-8, as the texts of the game) at x (its left, centre or right), y its top ; its width
		int text(const std::string& value, const TextStyle& style, int x, int y, Align align = Align::Left) const;
		// A 9-slice panel of the theme (Theme::panel)
		void panel(const std::string& image, const ska::Rectangle& box, int corner = 16, unsigned char alpha = 255) const;
		// A bar in the groove of the theme : box is the inside of the groove
		void bar(const ska::Rectangle& box, const Bar& bar) const;
		// An image (or a part of it), scaled around its centre or its bottom centre, faded
		void image(const ska::Texture& texture, int x, int y, const ska::Rectangle* clip = nullptr, float scale = 1.F, unsigned char alpha = 255, bool anchorBottom = false) const;

	private:
		ska::Renderer& m_renderer;
	};

	// The lines of a text ("\n" : a new line) cut to "width" letters (UTF-8 : a letter is not cut)
	std::vector<std::string> Wrap(const std::string& text, std::size_t width);

	// A drawing at a fixed priority (DrawableContainer) : it draws with a canvas when the frame is drawn
	class Layer : public ska::DrawableFixedPriority {
	public:
		explicit Layer(std::function<void(Canvas&)> draw, int priority = 0);
		void render(ska::Renderer& renderer) const override;
	private:
		std::function<void(Canvas&)> m_draw;
	};

	// A vertical list of choices for a screen that draws itself : up / down (W S), Return or Space, the mouse over the
	// boxes of the choices (given by the screen as it lays them out)
	class ChoiceList {
	public:
		void setCount(std::size_t count);
		std::size_t selected() const { return m_selected; }
		void select(std::size_t index) { if (index < m_count) { m_selected = index; } }
		// The keys of a frame (InputKeyEvent) : true when the selected choice is chosen
		bool keys(const ska::InputContextManager& icm);
		// The mouse of a frame (InputMouseEvent) over the boxes of the choices : true when one is clicked (selected)
		bool mouse(const ska::InputContextManager& icm, const std::vector<ska::Rectangle>& boxes);
	private:
		std::size_t m_count = 0;
		std::size_t m_selected = 0;
		ska::InputToggleContainer m_lastToggles;
		bool m_lastAction = true; // the key that opened the screen is not a choice
		bool m_lastClick = true;
	};
}
