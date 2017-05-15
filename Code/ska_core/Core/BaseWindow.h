#pragma once

namespace ska {
	class BaseWindow {
	public:
		BaseWindow(unsigned int w, unsigned int h);

		unsigned int getWidth() const;
		unsigned int getHeight() const;
		void resize(unsigned int w, unsigned int h);

		virtual ~BaseWindow() = default;

	private:
		unsigned int m_height;
		unsigned int m_width;

	};
}
