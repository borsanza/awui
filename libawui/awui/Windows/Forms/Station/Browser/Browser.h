#pragma once

#include <awui/Windows/Forms/Gradient.h>

namespace awui::Windows::Forms::Station::Browser {
	class Page;

	class Browser : public Control {
		private:
			Gradient m_gradientUp;
			Gradient m_gradientBottom;
			Gradient m_gradientLeft;
			Gradient m_gradientRight;
			Page * m_page;

		public:
			Browser();
			virtual ~Browser() = default;

			void SetPage(Page * page);
			Page * GetPage() const { return m_page; };

			virtual void OnTick(float deltaSeconds) override;
	};
}
