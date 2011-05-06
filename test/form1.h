// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/awuiForm.h>

class Form1 : public awuiForm {
public:
	Form1();

private:
	void InitializeComponent();
	void OnPaint(awuiGraphics * g);
};