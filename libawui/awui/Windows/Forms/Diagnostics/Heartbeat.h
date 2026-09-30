#pragma once

#include <awui/Windows/Forms/Control.h>

namespace awui::Windows::Forms::Diagnostics {
	class Heartbeat : public Control {
	  private:
		bool m_heartbeat;

	  public:
		Heartbeat();
		virtual ~Heartbeat();

		virtual void OnPaint(OpenGL::GL *gl);
		virtual void OnRemoteHeartbeat();
	};
} // namespace awui::Windows::Forms::Diagnostics
