#pragma once

#include <awui/Windows/Forms/Events/EventArgs.h>

namespace awui::Windows::Forms {
	class Control;
}

namespace awui::Windows::Forms::Events {
	class MouseEventArgs : public EventArgs {
	  private:
		friend class awui::Windows::Forms::Control;

		int m_x, m_y;
		int m_delta;
		int m_clicks;
		int m_button;

	  private:
		void SetX(int x);
		void SetY(int y);
		void SetLocation(int x, int y);
		void SetDelta(int delta);
		void SetClicks(int clicks);
		void SetButton(int button);

	  public:
		MouseEventArgs();
		virtual ~MouseEventArgs() = default;

		int GetX() const;
		int GetY() const;
		void GetLocation(int &x, int &y);
		int GetDelta() const;
		int GetClicks() const;
		int GetButton() const;
	};
} // namespace awui::Windows::Forms::Events
