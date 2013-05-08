// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Stats.h"

#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/TimeSpan.h>
#include <awui/Windows/Forms/ControlCollection.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms::Statistics;

Stats* Stats::pinstance = 0;

Stats::Stats() {
	this->SetBackColor(Color::FromArgb(0, 0, 0));
	this->used = 0;
	this->idle = TimeSpan(this->used.TicksPerSecond / 60.0f);
	this->total = this->idle;
	this->beforeSync = DateTime::GetNow();
	this->beforeSync_last = DateTime::GetNow();
	this->afterSync = DateTime::GetNow();

	Font font = Font("Monospace", 14);
	this->labelidle.SetFont(&font);
	this->labelused.SetFont(&font);
	this->labeltotal.SetFont(&font);
	this->spinner.SetDock(DockStyle::Right);
	this->labelidle.SetDock(DockStyle::Bottom);
	this->labelused.SetDock(DockStyle::Bottom);
	this->labeltotal.SetDock(DockStyle::Bottom);
	this->labelidle.SetTextAlign(ContentAlignment::MiddleRight);
	this->labelused.SetTextAlign(ContentAlignment::MiddleRight);
	this->labeltotal.SetTextAlign(ContentAlignment::MiddleRight);
	Color backColor = Color::FromArgb(0, 0, 0, 0);
	Color foreColor = Color::FromArgb(255, 255, 255);
	this->spinner.SetBackColor(backColor);
	this->labelidle.SetBackColor(backColor);
	this->labelused.SetBackColor(backColor);
	this->labeltotal.SetBackColor(backColor);
	this->spinner.SetForeColor(foreColor);
	this->labelidle.SetForeColor(foreColor);
	this->labelused.SetForeColor(foreColor);
	this->labeltotal.SetForeColor(foreColor);

	this->GetControls()->Add(&this->spinner);
	this->GetControls()->Add(&this->labeltotal);
	this->GetControls()->Add(&this->labelidle);
	this->GetControls()->Add(&this->labelused);
	this->SetHeight(72);
}

Stats::~Stats() {
}

void Stats::SetTimeBeforeVSync() {
	this->beforeSync_last = this->beforeSync;
	this->beforeSync = DateTime::GetNow();

	this->used = TimeSpan(beforeSync.GetTicks() - afterSync.GetTicks());
	this->idle = TimeSpan(afterSync.GetTicks() - beforeSync_last.GetTicks());
	this->total = TimeSpan(beforeSync.GetTicks() - beforeSync_last.GetTicks());
}

awui::TimeSpan Stats::GetIdle() {
	return this->idle;
}

void Stats::SetTimeAfterVSync() {
	this->afterSync = DateTime::GetNow();
}

void Stats::OnTick() {
	this->labelused.SetText(String("Used: ") + this->used.ToString());
	this->labelidle.SetText(String("Idle: ") + this->idle.ToString());
	this->labeltotal.SetText(String("Total: ") + this->total.ToString());
}

Stats* Stats::Instance() {
	if (pinstance == 0)
    pinstance = new Stats;

  return pinstance;
}
