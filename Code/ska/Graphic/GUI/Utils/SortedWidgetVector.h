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
			if (m_lastVisibleHandledWidget + 1 < m_data.size() && m_data.back()->isVisible()) {
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

		T& back() {
			return m_data.back();
		}

		~SortedWidgetVector() = default;
		void clear() {
			m_data.clear();
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

	private:
		std::vector<T> m_data;
		int m_lastVisibleHandledWidget;
	};
}
