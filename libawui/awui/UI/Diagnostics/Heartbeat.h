#pragma once

#include <awui/UI/Control.h>

namespace awui::UI::Diagnostics {
	class Heartbeat : public Control {
	  private:
		bool m_heartbeat;

	  public:
		Heartbeat();
		virtual ~Heartbeat();

		virtual void OnPaint(OpenGL::GL *gl);
		virtual void OnRemoteHeartbeat();
	};
} // namespace awui::UI::Diagnostics
