// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/awuiForm.h>

namespace awui {
	namespace Effects {
		class Effect;
	}

	namespace Collections {
		class ArrayList;
	}
}

class awuiButton;
class awuiPanel;
class awuiSplitContainer;

class Form1 : public awuiForm {
private:
	awuiSplitContainer * _splitter;
	awuiPanel * _panel;

	awui::Collections::ArrayList * _buttons;
	awui::Collections::ArrayList * _effects;

public:
	Form1();
	~Form1();

private:
	void InitializeComponent();
	void AddButtonEffect(awui::Effects::Effect * effect);

	virtual void OnTick();
};
