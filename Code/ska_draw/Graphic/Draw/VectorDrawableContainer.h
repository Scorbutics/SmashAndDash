#pragma once
#include <vector>
#include <functional>

#include "DrawableContainer.h"

namespace ska {
	class VectorDrawableContainer :
		public DrawableContainer
	{
	public:
		VectorDrawableContainer();
		virtual void reserve(const unsigned int size);
		std::vector<const Drawable*>::iterator begin();
		std::vector<const Drawable*>::iterator end();
		std::vector<const Drawable*>::const_iterator cbegin() const;
		std::vector<const Drawable*>::const_iterator cend() const;


		virtual void clear() override;
		virtual void draw() override;
		virtual ~VectorDrawableContainer();
	
	protected:
		virtual void push(const Drawable& d) override;

	private:
		std::vector<const Drawable*> m_data;
	};
}
