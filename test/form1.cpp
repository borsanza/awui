// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "form1.h"
#include <awui/awuiGraphics.h>
#include <awui/awuiColor.h>
#include <awui/awuiButton.h>
#include <awui/awuiSplitContainer.h>
#include <awui/awuiControlCollection.h>
#include <awui/awuiControl.h>
#include <awui/awuiPen.h>
#include <awui/awuiPanel.h>
#include <awui/animation/effects.h>

extern "C" {
	#include <aw/sysgl.h>
}

Form1::Form1() {
	this->InitializeComponent();
}

Form1::~Form1() {
}

void Form1::InitializeComponent() {
	awuiButton * button;

	button = new awuiButton();
	button->SetDock(awuiControl::Top);
	button->SetText("Button Top");
	button->SetMinimumSize(awuiSize(75, 23));
	this->GetControls()->Add(button);

	button = new awuiButton();
	button->SetDock(awuiControl::Bottom);
	button->SetText("Button Bottom");
	button->SetMinimumSize(awuiSize(75, 23));
	this->GetControls()->Add(button);

	button = new awuiButton();
	button->SetDock(awuiControl::Left);
	button->SetText("Button Left");
	button->SetMinimumSize(awuiSize(75, 23));
	this->GetControls()->Add(button);

	button = new awuiButton();
	button->SetDock(awuiControl::Right);
	button->SetText("Button Right");
	button->SetMinimumSize(awuiSize(75, 23));
	this->GetControls()->Add(button);
/*
	button = new awuiButton();
	awuiButton * button2 = new awuiButton();

	button->SetDock(awuiControl::Fill);
	button->SetText("Button Splitter Left");
	button->SetMinimumSize(awuiSize(75, 23));
	button2->SetDock(awuiControl::Fill);
	button2->SetText("Button Splitter Right");
	button2->SetMinimumSize(awuiSize(75, 23));

	this->splitter = new awuiSplitContainer();
	this->splitter->SetName("SplitContainer");
	this->splitter->GetPanel1()->GetControls()->Add(button);

	button = new awuiButton();
	button->SetDock(awuiControl::None);
	button->SetBounds(10, 10, 20, 20);
	button->SetText("Button Right2");
	button->SetBackColor(awuiColor::FromArgb(255, 0, 255));
	this->splitter->GetPanel2()->GetControls()->Add(button2);
	this->splitter->GetPanel2()->GetControls()->Add(button);
	this->splitter->SetDock(awuiControl::Fill);
	this->splitter->SetOrientation(awuiSplitContainer::Vertical);

	this->GetControls()->Add(this->splitter);
*/
    this->panel = new awuiPanel();
   	this->panel->SetDock(awuiControl::Fill);
    this->panel->SetBackColor(awuiColor::FromArgb(0, 0, 0));

	this->GetControls()->Add(panel);

	this->buttonLinear = new awuiButton();
	this->buttonLinear->SetDock(awuiControl::None);
	this->buttonLinear->SetText("Linear");
	this->buttonLinear->SetWidth(75);
	this->panel->GetControls()->Add(this->buttonLinear);

	this->buttonSwing = new awuiButton();
	this->buttonSwing->SetDock(awuiControl::None);
	this->buttonSwing->SetText("Swing");
	this->buttonSwing->SetWidth(75);
	this->panel->GetControls()->Add(this->buttonSwing);

	this->buttonQuad = new awuiButton();
	this->buttonQuad->SetDock(awuiControl::None);
	this->buttonQuad->SetText("Quad");
	this->buttonQuad->SetWidth(75);
	this->panel->GetControls()->Add(this->buttonQuad);

	this->buttonCubic = new awuiButton();
	this->buttonCubic->SetDock(awuiControl::None);
	this->buttonCubic->SetText("Cubic");
	this->buttonCubic->SetWidth(75);
	this->panel->GetControls()->Add(this->buttonCubic);

	this->buttonQuart = new awuiButton();
	this->buttonQuart->SetDock(awuiControl::None);
	this->buttonQuart->SetText("Quart");
	this->buttonQuart->SetWidth(75);
	this->panel->GetControls()->Add(this->buttonQuart);

	this->buttonQuint = new awuiButton();
	this->buttonQuint->SetDock(awuiControl::None);
	this->buttonQuint->SetText("Quint");
	this->buttonQuint->SetWidth(75);
	this->panel->GetControls()->Add(this->buttonQuint);

	this->buttonExpo = new awuiButton();
	this->buttonExpo->SetDock(awuiControl::None);
	this->buttonExpo->SetText("Expo");
	this->buttonExpo->SetWidth(75);
	this->panel->GetControls()->Add(this->buttonExpo);

	this->buttonSine = new awuiButton();
	this->buttonSine->SetDock(awuiControl::None);
	this->buttonSine->SetText("Sine");
	this->buttonSine->SetWidth(75);
	this->panel->GetControls()->Add(this->buttonSine);

	this->buttonCirc = new awuiButton();
	this->buttonCirc->SetDock(awuiControl::None);
	this->buttonCirc->SetText("Circ");
	this->buttonCirc->SetWidth(75);
	this->panel->GetControls()->Add(this->buttonCirc);

	this->buttonElastic = new awuiButton();
	this->buttonElastic->SetDock(awuiControl::None);
	this->buttonElastic->SetText("Elastic");
	this->buttonElastic->SetWidth(75);
	this->panel->GetControls()->Add(this->buttonElastic);

	this->buttonBack = new awuiButton();
	this->buttonBack->SetDock(awuiControl::None);
	this->buttonBack->SetText("Back");
	this->buttonBack->SetWidth(75);
	this->panel->GetControls()->Add(this->buttonBack);

	this->buttonBounce = new awuiButton();
	this->buttonBounce->SetDock(awuiControl::None);
	this->buttonBounce->SetText("Bounce");
	this->buttonBounce->SetWidth(75);
	this->panel->GetControls()->Add(this->buttonBounce);
}

float Form1::recalc(float value, int sube, Effect * effect) {
/*
    EffectIn in;
    value = in.calculate(value, effect);
*/
/*
    EffectOut out;
    value = 1.0f - out.calculate(value, effect);
*/

    EffectInOut inout;
    value = inout.calculate(value, effect);

    if (!sube)
        value = 1.0f - value;

    return value;
}

void Form1::OnTick() {
    static float py = 0.0f;
    static int sube = 1;
    py++;
    if (py > 500.0f) {
        py = 0.0f;
        sube = !sube;
    }

	float value = py / 500.0f;

    int width = this->panel->GetWidth() - 75;

	EffectLinear linearEffect;
	float value2;
	value2 = this->recalc(value, sube, &linearEffect);
	this->buttonLinear->SetLocation(width * value2, 10);

	EffectSwing swingEffect;
	value2 = this->recalc(value, sube, &swingEffect);
	this->buttonSwing->SetLocation(width * value2, 40);

	EffectQuad quadEffect;
	value2 = this->recalc(value, sube, &quadEffect);
	this->buttonQuad->SetLocation(width * value2, 70);

	EffectCubic cubicEffect;
	value2 = this->recalc(value, sube, &cubicEffect);
	this->buttonCubic->SetLocation(width * value2, 100);

	EffectQuart quartEffect;
	value2 = this->recalc(value, sube, &quartEffect);
	this->buttonQuart->SetLocation(width * value2, 130);

	EffectQuint quintEffect;
	value2 = this->recalc(value, sube, &quintEffect);
	this->buttonQuint->SetLocation(width * value2, 160);

	EffectExpo expoEffect;
	value2 = this->recalc(value, sube, &expoEffect);
	this->buttonExpo->SetLocation(width * value2, 190);

	EffectSine sineEffect;
	value2 = this->recalc(value, sube, &sineEffect);
	this->buttonSine->SetLocation(width * value2, 220);

	EffectCirc circEffect;
	value2 = this->recalc(value, sube, &circEffect);
	this->buttonCirc->SetLocation(width * value2, 250);

	EffectElastic elasticEffect;
	value2 = this->recalc(value, sube, &elasticEffect);
	this->buttonElastic->SetLocation(width * value2, 280);

	EffectBack backEffect;
	value2 = this->recalc(value, sube, &backEffect);
	this->buttonBack->SetLocation(width * value2, 310);

	EffectBounce bounceEffect;
	value2 = this->recalc(value, sube, &bounceEffect);
	this->buttonBounce->SetLocation(width * value2, 340);
}
