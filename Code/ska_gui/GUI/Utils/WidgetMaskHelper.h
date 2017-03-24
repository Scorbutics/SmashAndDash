#pragma once

namespace ska {
	class WidgetMaskHelper {
	private:
		WidgetMaskHelper() = default;

	public:
		template <class T>
		static unsigned int getMask() {
			return (WidgetMaskHelper::template getWidgetMaskCounter<T>());
		}

	private:
		class WStaticCounterGlobal {
		protected:
			static unsigned int getCounterAndInc() {
				return m_typeCounter++;
			}

			static unsigned int m_typeCounter;
		};

		template <class T>
		class WStaticCounterSpecialized : public WStaticCounterGlobal {
		public:
			WStaticCounterSpecialized() {
				m_mask = getCounterAndInc();
			}

			unsigned int getMask() const{
				return m_mask;
			}

		private:
			unsigned int m_mask;
		};

		template <class T>
		static unsigned int getWidgetMaskCounter() {
			static WStaticCounterSpecialized<T> wc;
			return wc.getMask();
		}
	};
}
