#pragma once
#include <memory>
namespace ska {
	class Component {
	public:
		Component(){}
		virtual ~Component(){}
	};
	typedef std::unique_ptr<Component> ComponentPtr;
}