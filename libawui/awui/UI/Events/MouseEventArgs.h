#pragma once

#include <awui/UI/Events/EventArgs.h>

namespace awui::UI {
	class Control;
}

namespace awui::UI::Events {
	class MouseEventArgs : public EventArgs {
	  private:
		friend class awui::UI::Control;

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
} // namespace awui::UI::Events
