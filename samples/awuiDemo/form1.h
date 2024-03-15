#pragma once

#include <awui/Windows/Forms/Form.h>

namespace awui::Windows::Forms {
	class Bitmap;
}

class Form1 : public awui::Windows::Forms::Form {
  private:
	awui::Windows::Forms::Bitmap *m_bitmap2;

  public:
	Form1();
	virtual ~Form1() = default;

  private:
	void InitializeComponent();
};
