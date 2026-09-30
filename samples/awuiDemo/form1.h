#pragma once

#include <awui/UI/Form.h>

namespace awui::UI {
	class Bitmap;
}

class Form1 : public awui::UI::Form {
  private:
	awui::UI::Bitmap *m_bitmap2;

  public:
	Form1();
	virtual ~Form1() = default;

  private:
	void InitializeComponent();
};
