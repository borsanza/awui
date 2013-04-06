// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/awuiForm.h>

class awuiButton;
class awuiPanel;
class awuiSplitContainer;
class awuiArrayList;
class Effect;

class Form1 : public awuiForm {
private:
	awuiSplitContainer * _splitter;
	awuiPanel * _panel;

	awuiArrayList * _buttons;
	awuiArrayList * _effects;

public:
	Form1();
	~Form1();

private:
	void InitializeComponent();
	void AddButtonEffect(Effect * effect);

	virtual void OnTick();
};
