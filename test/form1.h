// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/awuiForm.h>

class awuiSplitContainer;

class Form1 : public awuiForm {
private:
	awuiSplitContainer * splitter;

public:
	Form1();
	~Form1();

private:
	void InitializeComponent();

	float GetAnimationValue();
	virtual void OnTick();
};