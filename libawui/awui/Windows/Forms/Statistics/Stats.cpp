// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Stats.h"

#include <awui/Convert.h>
#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/Math.h>
#include <awui/TimeSpan.h>
#include <awui/Windows/Forms/ControlCollection.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms::Statistics;

Stats* Stats::pinstance = 0;

Stats::Stats() {
	this->fps = 0;
	this->fpsCalculated = 0;
	this->lastSecond = -1;
	// this->percent = 0;
	this->drawedControls = 0;

	this->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	this->idle = TimeSpan(0.0f);
	this->beforeSync = DateTime::GetNow();
	this->afterSync = DateTime::GetNow();

	Font font = Font("Monospace", 14);

	// this->labelPercent = new Label();
	this->labelControls = new Label();
	this->labelFPS = new Label();
	this->heartbeat = new Heartbeat();
	this->spinner = new Spinner();

	// this->labelPercent->SetFont(&font);
	this->labelControls->SetFont(&font);
	this->labelFPS->SetFont(&font);

	this->spinner->SetDock(DockStyle::Right);
	this->heartbeat->SetDock(DockStyle::Left);
	this->labelControls->SetDock(DockStyle::Right);
	// this->labelPercent->SetDock(DockStyle::Right);
	this->labelFPS->SetDock(DockStyle::Right);
	this->labelControls->SetTextAlign(ContentAlignment::MiddleRight);
	// this->labelPercent->SetTextAlign(ContentAlignment::MiddleRight);
	this->labelFPS->SetTextAlign(ContentAlignment::MiddleCenter);
	Color backColor = Color::FromArgb(0, 0, 0, 0);
	Color foreColor = Color::FromArgb(255, 255, 255);
	this->heartbeat->SetBackColor(backColor);
	this->spinner->SetBackColor(backColor);
	this->labelControls->SetBackColor(backColor);
	// this->labelPercent->SetBackColor(backColor);
	this->labelFPS->SetBackColor(backColor);
	this->heartbeat->SetForeColor(foreColor);
	this->spinner->SetForeColor(foreColor);
	this->labelControls->SetForeColor(foreColor);
	// this->labelPercent->SetForeColor(foreColor);
	this->labelFPS->SetForeColor(foreColor);

	this->labelControls->SetWidth(150);
	this->labelFPS->SetWidth(90);
	// this->labelPercent->SetWidth(70);

	this->GetControls()->Add(this->heartbeat);
	this->GetControls()->Add(this->spinner);
	this->GetControls()->Add(this->labelFPS);
	// this->GetControls()->Add(this->labelPercent);
	// this->GetControls()->Add(this->labelControls);

	// this->SetHeight(24);
	this->SetHeight(0);
}

Stats::~Stats() {
}

void Stats::SetTimeBeforeIddle() {
	DateTime beforeSync_last = this->beforeSync;
	this->beforeSync = DateTime::GetNow();

	this->idle = TimeSpan(this->afterSync.GetTicks() - beforeSync_last.GetTicks());

	this->timeUsed += this->beforeSync.GetTicks() - this->afterSync.GetTicks();
}

awui::TimeSpan Stats::GetIdle() {
	return this->idle;
}

void Stats::SetTimeAfterIddle() {
	this->afterSync = DateTime::GetNow();

	int second = this->afterSync.GetSecond();
	this->fps++;

	if (this->lastSecond != second) {
		this->lastSecond = second;
		float total = ((this->afterSync.GetTicks() - this->lastTime.GetTicks()) / 10000000.0f);

		if (total != 0) {
			float calc = Math::Round((this->fps / total) * 10.0f) / 10.0f;

			if (this->fpsCalculated != calc) {
				this->fpsCalculated = calc;
				this->labelFPS->SetText(Convert::ToString(this->fpsCalculated, 1) + String(" FPS"));
			}
		}

		this->fps = 0;
		this->lastTime = this->afterSync;

/*
		if (total != 0) {
			float calc = Math::Round((this->timeUsed / 10000000.0f) / total * 10000.0f) / 100.0f;
			if (calc != this->percent) {
				this->percent = calc;
				this->labelPercent->SetText(Convert::ToString(this->percent, 2) + String("%"));
			}
		}
*/

		this->timeUsed = 0;
	}
}

Stats* Stats::Instance() {
	if (pinstance == 0)
    pinstance = new Stats;

  return pinstance;
}

void Stats::OnRemoteHeartbeat() {
	this->heartbeat->OnRemoteHeartbeat();
}

void Stats::SetDrawedControls(int drawedControls) {
	if (this->drawedControls != drawedControls) {
		this->drawedControls = drawedControls;
		this->labelControls->SetText(Convert::ToString(this->drawedControls) + String(" widgets"));
	}
}
