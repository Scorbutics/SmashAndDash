#pragma once
#include <memory>
namespace ska {
	class Component {
	public:
		Component(){}
		
		void setMask(const unsigned int m) {
			m_mask = m;
		}

		virtual ~Component(){}
	private:
		unsigned int m_mask;
	};
	typedef std::unique_ptr<Component> ComponentPtr;
}