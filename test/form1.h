// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/awuiForm.h>

class awuiPen;

class Form1 : public awuiForm {
private:
	awuiPen * pen1;

public:
	Form1();
	~Form1();

private:
	float GetAnimationValue();
	void InitializeComponent();
	void OnPaint(awuiGraphics * g);
};