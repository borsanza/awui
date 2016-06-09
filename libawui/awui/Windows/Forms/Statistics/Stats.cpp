// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Stats.h"

#include <awui/Convert.h>
//#include <awui/Drawing/Color.h>
#include <awui/Drawing/Font.h>
#include <awui/Math.h>
//#include <awui/TimeSpan.h>
#include <awui/Windows/Forms/ControlCollection.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms::Statistics;

Stats* Stats::_instance = 0;

Stats::Stats() {
#if defined(SHOW_FPS) || defined(SHOW_PERCENT)
	this->_lastSecond = -1;
#endif // defined

	this->SetBackColor(Color::FromArgb(0, 0, 0, 0));
	this->_idle = TimeSpan(0.0f);
	this->_beforeSync = DateTime::GetNow();
	this->_afterSync = DateTime::GetNow();

	Font font = Font("Monospace", 14);
	Color backColor = Color::FromArgb(0, 0, 0, 0);
	Color foreColor = Color::FromArgb(255, 255, 255);

#ifdef SHOW_HEARTBEAT
	this->_heartbeat = new Heartbeat();
	this->_heartbeat->SetDock(DockStyle::Left);
	this->_heartbeat->SetBackColor(backColor);
	this->_heartbeat->SetForeColor(foreColor);
	this->GetControls()->Add(this->_heartbeat);
#endif // SHOW_HEARTBEAT

#ifdef SHOW_SPINNER
	this->_spinner = new Spinner();
	this->_spinner->SetDock(DockStyle::Right);
	this->_spinner->SetBackColor(backColor);
	this->_spinner->SetForeColor(foreColor);
	this->GetControls()->Add(this->_spinner);
#endif // SHOW_SPINNER

#ifdef SHOW_FPS
	this->_fps = 0;
	this->_fpsCalculated = 0;
	this->_labelFPS = new Label();
	this->_labelFPS->SetFont(font);
	this->_labelFPS->SetDock(DockStyle::Right);
	this->_labelFPS->SetTextAlign(ContentAlignment::MiddleCenter);
	this->_labelFPS->SetBackColor(backColor);
	this->_labelFPS->SetForeColor(foreColor);
	this->_labelFPS->SetWidth(90);
	this->GetControls()->Add(this->_labelFPS);
#endif // SHOW_FPS

#ifdef SHOW_PERCENT
	this->_percent = 0;
	this->_labelPercent = new Label();
	this->_labelPercent->SetFont(font);
	this->_labelPercent->SetDock(DockStyle::Right);
	this->_labelPercent->SetTextAlign(ContentAlignment::MiddleRight);
	this->_labelPercent->SetBackColor(backColor);
	this->_labelPercent->SetForeColor(foreColor);
	this->_labelPercent->SetWidth(70);
	this->GetControls()->Add(this->_labelPercent);
#endif // SHOW_PERCENT

#ifdef SHOW_WIDGETS
	this->_drawedControls = 0;
	this->_labelControls = new Label();
	this->_labelControls->SetFont(font);
	this->_labelControls->SetDock(DockStyle::Right);
	this->_labelControls->SetTextAlign(ContentAlignment::MiddleRight);
	this->_labelControls->SetBackColor(backColor);
	this->_labelControls->SetForeColor(foreColor);
	this->_labelControls->SetWidth(150);
	this->GetControls()->Add(this->_labelControls);
#endif // SHOW_WIDGETS

	this->SetHeight(24);
}

Stats::~Stats() {
}

void Stats::SetTimeBeforeIddle() {
	DateTime beforeSync_last = this->_beforeSync;
	this->_beforeSync = DateTime::GetNow();

	this->_idle = TimeSpan(this->_afterSync.GetTicks() - beforeSync_last.GetTicks());

#ifdef SHOW_PERCENT
	this->_timeUsed += this->_beforeSync.GetTicks() - this->_afterSync.GetTicks();
#endif // SHOW_PERCENT
}

awui::TimeSpan Stats::GetIdle() {
	return this->_idle;
}

void Stats::SetTimeAfterIddle() {
	this->_afterSync = DateTime::GetNow();

#ifdef SHOW_FPS
	this->_fps++;
#endif // SHOW_FPS

#if defined(SHOW_FPS) || defined(SHOW_PERCENT)
	int second = this->_afterSync.GetSecond();
	if (this->_lastSecond != second) {
		this->_lastSecond = second;
		float total = ((this->_afterSync.GetTicks() - this->_lastTime.GetTicks()) / 10000000.0f);

#ifdef SHOW_FPS
		if (total != 0) {
			float calc = Math::Round((this->_fps / total) * 10.0f) / 10.0f;

			if (this->_fpsCalculated != calc) {
				this->_fpsCalculated = calc;
				this->_labelFPS->SetText(Convert::ToString(this->_fpsCalculated, 1) + String(" FPS"));
			}
		}

		this->_fps = 0;
#endif // SHOW_FPS

		this->_lastTime = this->_afterSync;

#ifdef SHOW_PERCENT
		if (total != 0) {
			float calc = Math::Round((this->_timeUsed / 10000000.0f) / total * 10000.0f) / 100.0f;
			if (calc != this->_percent) {
				this->_percent = calc;
				this->_labelPercent->SetText(Convert::ToString(this->_percent, 2) + String("%"));
			}
		}
		this->_timeUsed = 0;
#endif // SHOW_PERCENT
	}
#endif // defined
}

Stats* Stats::Instance() {
	if (Stats::_instance == 0)
		Stats::_instance = new Stats;

	return Stats::_instance;
}

void Stats::OnRemoteHeartbeat() {
#ifdef SHOW_HEARTBEAT
	this->_heartbeat->OnRemoteHeartbeat();
#endif // SHOW_HEARTBEAT
}

void Stats::SetDrawedControls(int drawedControls) {
#ifdef SHOW_WIDGETS
	if (this->_drawedControls != drawedControls) {
		this->_drawedControls = drawedControls;
		this->_labelControls->SetText(Convert::ToString(this->_drawedControls) + String(" widgets"));
	}
#endif // SHOW_WIDGETS
}
