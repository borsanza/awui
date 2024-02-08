#pragma once

#include <awui/Windows/Forms/Button.h>

using namespace awui;
using namespace awui::Windows::Forms;

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

class SettingsWidget : public Button {
	private:
		Bitmap * _bitmap;

	public:
		SettingsWidget();
		virtual ~SettingsWidget();

		virtual void OnPaint(OpenGL::GL* gl);
};
