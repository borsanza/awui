#pragma once

#include <awui/UI/Control.h>

namespace awui::UI::Diagnostics {
	class Spinner : public Control {
	  private:
		int m_position;

	  public:
		Spinner();
		virtual ~Spinner();

		virtual void OnTick(float deltaSeconds);
		virtual void OnPaint(OpenGL::GL *gl);
	};
} // namespace awui::UI::Diagnostics
