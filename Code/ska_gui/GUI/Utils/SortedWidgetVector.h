#pragma once
#include <vector>

namespace ska {
	class Widget;

	template <class T>
	class SortedWidgetVector {
	public:

		SortedWidgetVector() : m_lastVisibleHandledWidget(-1) {
		}

		void push_back(T&& w) {
			m_data.push_back(std::move(w));
			if (static_cast<std::size_t>(m_lastVisibleHandledWidget + 1) < m_data.size() && m_data.back()->isVisible()) {
				std::swap(m_data[m_data.size() - 1], m_data[m_lastVisibleHandledWidget + 1]);
				m_lastVisibleHandledWidget++;
			}
		}

		std::size_t size() const {
			return m_data.size();
		}

		typename std::vector<T>::iterator begin() {
			return m_data.begin();
		}

		typename std::vector<T>::iterator end() {
			return m_data.end();
		}

		typename std::vector<T>::const_iterator cbegin() const {
			return m_data.cbegin();
		}

		typename std::vector<T>::const_iterator cend() const {
			return m_data.cend();
		}

		bool organize(T& w, std::size_t cursor) {
			if (!w->isVisible()) {
				if (!isVisibleAtIndex(cursor)) {
					/* If we are at the end of visible widgets, we stop propagation */
					return true;
				}
				if (m_lastVisibleHandledWidget >= 0) {
					if(static_cast<std::size_t>(m_lastVisibleHandledWidget) >= m_data.size()) {
						auto i = static_cast<int>(m_data.size() - 1);
						// ReSharper disable once CppPossiblyErroneousEmptyStatements
						//TODO replace with find
						for (; !m_data[i]->isVisible() && i >= 0; i--);
						if (i >= 0) {
							m_lastVisibleHandledWidget = i;
						} else {
							m_lastVisibleHandledWidget = 0;
							return true;
						}
					}

					/* Otherwise this widget just turned invisible, so we swap it with the last visible widget */
					std::swap(m_data[cursor], m_data[static_cast<std::size_t>(m_lastVisibleHandledWidget)]);
					m_lastVisibleHandledWidget--;
				}
			} else if (static_cast<int>(cursor) > m_lastVisibleHandledWidget && static_cast<std::size_t>(m_lastVisibleHandledWidget + 1) < m_data.size()) {
				std::swap(m_data[cursor], m_data[static_cast<std::size_t>(m_lastVisibleHandledWidget + 1)]);
				m_lastVisibleHandledWidget++;
			}
			return false;
		}

		typename std::vector<T>::iterator erase (typename std::vector<T>::const_iterator position) {
		    return m_data.erase(position);
		}

		T& back() {
			return m_data.back();
		}

		~SortedWidgetVector() = default;
		void clear() {
			m_data.clear();
		}

		void pop_back() {
		    m_data.pop_back();
		}

		bool empty() const {
			return m_data.empty();
		}

		Widget* at(std::size_t index) {
			return m_data.at(index);
		}

		bool isVisibleAtIndex(std::size_t cursor) const {
			return static_cast<int>(cursor) <= m_lastVisibleHandledWidget;
		}

		bool operator==(const SortedWidgetVector<T>& v) const {
			return m_data == v.m_data;
		}

	private:
		std::vector<T> m_data;
		int m_lastVisibleHandledWidget;
	};
}
