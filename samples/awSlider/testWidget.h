#pragma once

#include <awui/UI/Button.h>

namespace awui {
	namespace Drawing {
		class Image;
	}

	namespace OpenGL {
		class GL;
	}
} // namespace awui

using namespace awui::UI;
using namespace awui;

class TestWidget : public Button {
  private:
	Drawing::Image *m_image;

  public:
	TestWidget();
	virtual ~TestWidget();

	virtual void OnTick(float deltaSeconds);

	virtual void OnPaint(OpenGL::GL *gl);
};
