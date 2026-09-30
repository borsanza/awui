#ifndef _FORMTIS100_H
#define _FORMTIS100_H

#include <awui/String.h>
#include <awui/UI/Form.h>

using namespace awui::UI;

class FormTIS100 : public awui::UI::Form {
  private:
	void InitializeComponent();

  public:
	FormTIS100();
	virtual ~FormTIS100();

	virtual void OnTick(float deltaSeconds);
};

#endif
