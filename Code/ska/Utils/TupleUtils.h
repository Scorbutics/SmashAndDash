#pragma once
#include <utility>
#include <functional>

namespace ska {
	namespace meta {
		template<int ...>
		struct SeqList { };

		template<int N, int ...S>
		struct Gens : Gens<N - 1, N - 1, S...> { };

		template<int ...S>
		struct Gens<0, S...> {
			typedef SeqList<S...> type;
		};

		template<unsigned int ArgsSizeof>
		using SeqRange = typename Gens<ArgsSizeof>::type;

		template<typename T, typename F, int... Is>
		static void for_each(T&& t, F f, SeqList<Is...> seq) {
			auto l = { (f(std::get<Is>(t)), 0)... };
		}


		template<typename... Ts, typename F>
		static void for_each_in_tuple(std::tuple<Ts...>& t, F f) {
			for_each(t, f, SeqRange<sizeof...(Ts)>());
		}

		template<int Index, class Search, class First, class... Types>
		struct get_internal
		{
			typedef typename get_internal<Index + 1, Search, Types...>::type type;
			static const int index = Index;
		};

		template<int Index, class Search, class... Types>
		struct get_internal<Index, Search, Search, Types...>
		{
			typedef get_internal type;
			static const int index = Index;
		};

		template<class T, class... Types>
		static T& get(std::tuple<Types...>& tuple) {
			return std::get<get_internal<0, T, Types...>::type::index>(tuple);
		}

		namespace detailindex {
			/*template <class EL, class T, std::size_t N>
			struct get_index_from_tuple_impl {

				static EL& visit(T& t, std::size_t index) {
					if (N - 1 == index) {
						auto& found = std::get<N - 1>(t);
						return found;
					}
					return get_index_from_tuple_impl<EL, N - 1>::visit(t, index);
				}
			};

			template <class EL, class T>
			struct get_index_from_tuple_impl<EL, T, 0> {
				static EL& visit(T& t, std::size_t index) {
					throw std::runtime_error("Cannot find tuple element");
				}
			};*/


			template <class F, class T, std::size_t N>
			struct visit_index_from_tuple_impl {

				static void visit(F& f, T& t, std::size_t index) {
					if (N - 1 == index) {
						auto& found = std::get<N - 1>(t);
						f(found);
					}
					else {
						visit_index_from_tuple_impl<F, T, N - 1>::visit(f, t, index);
					}
				}
			};

			template <class F, class T>
			struct visit_index_from_tuple_impl<F, T, 0> {
				static void visit(F& f, T& t, std::size_t index) {
				}
			};
		}

		/*template <class T, class... Args>
		T& get_element_by_index(std::tuple<Args...>& t, std::size_t index) {
			return detailindex::get_index_from_tuple_impl<T, std::tuple<Args...>, sizeof ...(Args)>::visit(t, index);
		}
*/
		template<class F, class ...Args>
		void visit_element_at_index(std::tuple<Args...>& t, std::size_t index, F& f) {
			detailindex::visit_index_from_tuple_impl<F, std::tuple<Args...>, sizeof ...(Args)>::visit(f, t, index);
		}
	}
}