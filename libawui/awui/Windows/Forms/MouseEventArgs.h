#pragma once

#include <awui/Windows/Forms/EventArgs.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class MouseEventArgs : public EventArgs {
				private:
					friend class Control;

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
					virtual ~MouseEventArgs();

					virtual bool IsClass(Classes objectClass) const override;

					int GetX();
					int GetY();
					void GetLocation(int &x, int &y);
					int GetDelta();
					int GetClicks();
					int GetButton();
			};
		}
	}
}
