#include "BaseWindow.h"

#define TAILLEBLOCFENETRE 32
#define TAILLEECRANMINX TAILLEBLOCFENETRE*15
#define TAILLEECRANMINY TAILLEBLOCFENETRE*13

ska::BaseWindow::BaseWindow(unsigned int w, unsigned int h) : 
	m_height(h < TAILLEECRANMINY ? TAILLEECRANMINY : h),
	m_width(w < TAILLEECRANMINX ? TAILLEECRANMINX : w) {
}

unsigned int ska::BaseWindow::getWidth() const {
	return m_width;
}

unsigned int ska::BaseWindow::getHeight() const {
	return m_height;
}

void ska::BaseWindow::resize(unsigned int w, unsigned int h) {
	m_width = w;
	m_height = h;
}
