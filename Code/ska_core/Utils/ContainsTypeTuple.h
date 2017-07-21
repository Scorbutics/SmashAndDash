#pragma once
#include <type_traits>

namespace ska {
    namespace meta {

        namespace detail {
            template <class TypeToFind, class FirstType, class ... RemainingTypes>
            struct containsImpl {
                static constexpr bool value = containsImpl<TypeToFind, FirstType>::value || containsImpl<TypeToFind, RemainingTypes...>::value;
            };

            template <class TypeToFind, class Type>
            struct containsImpl<TypeToFind, Type> {
                static constexpr bool value = std::is_same<TypeToFind, Type>::value;
            };

        }

        template <class TypeToFind, class ... Types>
        struct contains {
            static constexpr bool value = detail::containsImpl<TypeToFind, Types...>::value;
        };

		template <class TypeToFind>
		struct contains <TypeToFind>{
			static constexpr bool value = false;
		};

        #if __cplusplus > 201402L
            //CPP 17
            template <class TypeToFind, template<class> class Container, class ... Types>
            struct contains<TypeToFind, Container<Types...>> {
                static constexpr bool value = detail::containsImpl<TypeToFind, Types...>::value;
            };
        #else
            //CPP 14
            template <class TypeToFind, class ... Types>
            struct contains<TypeToFind, std::tuple<Types...>> {
                static constexpr bool value = detail::containsImpl<TypeToFind, Types...>::value;
            };
        #endif


    }
}
