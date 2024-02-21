#pragma once

#include <awui/Windows/Forms/Button.h>

namespace awui {
	namespace Drawing {
		class Image;
	}

	namespace OpenGL {
		class GL;
	}
}

using namespace awui::Windows::Forms;
using namespace awui;

class TestWidget : public Button {
	private:
		Drawing::Image * _image;

	public:
		TestWidget();
		virtual ~TestWidget();

//		virtual int IsClass(Classes::Enum objectClass) const;

		virtual void OnTick(float deltaSeconds);

		virtual void OnPaint(OpenGL::GL* gl);
};
