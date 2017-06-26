#pragma once
#include <memory>

namespace ska {
	namespace meta {

		template <typename T>
		struct un_unique_type {
			typedef T raw;
		};

		template <typename T>
		struct un_unique_type<std::unique_ptr<T>> {
			typedef T raw;
		};

		template <typename T>
		struct un_unique_type<std::unique_ptr<T>&> {
			typedef T raw;
		};

	}
}