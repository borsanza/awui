// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "test1.h"

#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/Effects/Effect.h>
#include <awui/Math.h>
#include <awui/Windows/Forms/Button.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/Panel.h>
#include <awui/Windows/Forms/SplitContainer.h>

using namespace awui::Drawing;
using namespace awui::Effects;
using namespace awui::Windows::Forms;

Test1::Test1() {
	m_buttons = NULL;
	m_effects = NULL;
	m_splitter = NULL;
	InitializeComponent();
}

void Test1::InitializeComponent() {
	SetBackColor(Color::FromArgb(0, 0, 0, 0));
	SetDock(DockStyle::Fill);

	m_splitter = new SplitContainer();
	m_splitter->SetName("SplitContainer");
	m_splitter->SetDock(DockStyle::Fill);
	m_splitter->SetOrientation(SplitContainer::Orientation::Vertical);
	m_splitter->SetSplitterDistance(630);

	AddWidget(m_splitter);

	Control * control1 = m_splitter->GetPanel1();
	Control * control2 = m_splitter->GetPanel2();
	control1->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	control2->SetBackColor(Color::FromArgb(0, 0, 0, 0));

	m_buttons = new awui::Collections::ArrayList();
	m_effects = new awui::Collections::ArrayList();

	AddButtonEffect(new EffectLinear(), control1, 5);
	AddButtonEffect(new EffectSwing(), control1);
	AddButtonEffect(new EffectQuad(), control1);
	AddButtonEffect(new EffectCubic(), control1);
	AddButtonEffect(new EffectQuart(), control1);
	AddButtonEffect(new EffectQuint(), control1);

	AddButtonEffect(new EffectExpo(), control2, 5);
	AddButtonEffect(new EffectSine(), control2);
	AddButtonEffect(new EffectCirc(), control2);
	AddButtonEffect(new EffectElastic(), control2);
	AddButtonEffect(new EffectBack(), control2);
	AddButtonEffect(new EffectBounce(), control2);
}

void Test1::AddButtonEffect(Effect * effect, Control * control, int posy) {
	static int y = 5;

	if (posy != -1)
		y = 5;

	Button * focused = nullptr;
	for (int i = 0; i<=2; i++) {
		Button * button = new Button();
		button->SetDock(DockStyle::None);
		button->SetText(effect->GetName());
		button->SetSize(150, 52);
		button->SetBackColor(Color::FromArgb(0, 0, 0, 0));
		button->SetForeColor(Color::FromArgb(255, 255, 255));
		button->SetFont(Font("Monospace", 20, FontStyle::Bold));
		button->SetTop(y);
		control->AddWidget(button);

		m_buttons->Add(button);
		m_effects->Add(effect);

		y += button->GetHeight();

		if (!focused) {
			focused = button;
		}
	}

	if (focused) {
		focused->SetFocus();
	}
}

void Test1::OnTick(float deltaSeconds) {
	static float py1 = 0.0f;
	static float py2 = 0.0f;
	static int sube1 = 1;
	static int sube2 = 1;
	py1++;
	py2++;

	// Controles 1
	Control * control = ((Control *)m_buttons->Get(0))->GetParent();
	int time = control->GetWidth() / 3;

	if (py1 > time) {
		py1 = 0.0f;
		sube1 = !sube1;
	}

	float value1 = py1 / time;

	control = ((Control *)m_buttons->Get(18))->GetParent();
	time = control->GetWidth() / 3;

	if (py2 > time) {
		py2 = 0.0f;
		sube2 = !sube2;
	}

	float value2 = py2 / time;

	for (int i=0; i<m_buttons->GetCount(); i++) {
		Effect * effect = (Effect *)m_effects->Get(i);
		Button * button = (Button *)m_buttons->Get(i);

		float value3 = 0.0f;

		switch (i % 3) {
			case 0:
				EffectIn in;
				value3 = in.Calculate(i<18?value1:value2, effect);
				break;
			case 1:
				EffectOut out;
				value3 = out.Calculate(i<18?value1:value2, effect);
				break;
			case 2:
				EffectInOut inout;
				value3 = inout.Calculate(i<18?value1:value2, effect);
				break;
		}

		if (i < 18) {
			if (!sube1)
				value3 = 1.0f - value3;
		} else {
			if (!sube2)
				value3 = 1.0f - value3;
		}

		int width = button->GetParent()->GetWidth() - button->GetWidth() - 10;
		int left = awui::Math::Round(width * value3 + 5);
		button->SetLeft(left);
	}
}
