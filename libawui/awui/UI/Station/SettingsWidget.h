#pragma once

#include <awui/UI/Button.h>

namespace awui::UI::Station {
	class SettingsWidget : public Button {
	  private:
		Bitmap *m_bitmap;

	  public:
		SettingsWidget();
		virtual ~SettingsWidget();

		virtual void OnPaint(OpenGL::GL *gl);

		String ToString() const override;
	};
} // namespace awui::UI::Station
