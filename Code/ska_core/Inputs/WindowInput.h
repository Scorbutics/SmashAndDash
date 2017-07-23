#pragma once

#include <SDL.h>
#include <bitset>

namespace ska {
	class WindowInput {

	public:
		WindowInput();
		~WindowInput() = default;
		void setSize(Sint32 data1, Sint32 data2);
		unsigned int getWidth() const;
		unsigned int getHeight() const;

	private:
		unsigned int m_width;
		unsigned int m_height;
	};
}
