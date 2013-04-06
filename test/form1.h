// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/awuiForm.h>

class awuiSplitContainer;
class awuiButton;
class awuiPanel;
class Effect;

class Form1 : public awuiForm {
private:
	awuiSplitContainer * splitter;
	awuiButton * buttonLinear;
	awuiButton * buttonSwing;
	awuiButton * buttonQuad;
	awuiButton * buttonCubic;
	awuiButton * buttonQuart;
	awuiButton * buttonQuint;
	awuiButton * buttonExpo;
	awuiButton * buttonSine;
	awuiButton * buttonCirc;
	awuiButton * buttonElastic;
	awuiButton * buttonBack;
	awuiButton * buttonBounce;
	awuiPanel * panel;

public:
	Form1();
	~Form1();

private:
	void InitializeComponent();

	float recalc(float value, int sube, Effect *effect);
	virtual void OnTick();
};
