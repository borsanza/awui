#pragma once

#include <awui/Windows/Forms/Button.h>

namespace awui::Windows::Forms::Station {
	class SettingsWidget : public Button {
	  private:
		Bitmap *m_bitmap;

	  public:
		SettingsWidget();
		virtual ~SettingsWidget();

		virtual void OnPaint(OpenGL::GL *gl);

		String ToString() const override;
	};
} // namespace awui::Windows::Forms::Station
