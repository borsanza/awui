#pragma once

#include <awui/Windows/Forms/Button.h>
#include <awui/Windows/Forms/Listeners/IButtonListener.h>

using namespace awui;
using namespace awui::Windows::Forms;
using namespace awui::Windows::Forms::Listeners;

namespace awui {
	namespace Windows {
		namespace Forms {
			class Bitmap;
		}
	}
		
	namespace OpenGL {
		class GL;
	}
}

class SettingsWidget : public Button, public IButtonListener {
	private:
		Bitmap * m_bitmap;

	public:
		SettingsWidget();
		virtual ~SettingsWidget();

		virtual void OnPaint(OpenGL::GL* gl);

		virtual void OnClick(Control* sender) override;
};
