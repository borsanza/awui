// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Stats.h"

#include <awui/Drawing/Font.h>
#include <awui/Math.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms::Statistics;

Stats *Stats::m_instance = 0;

Stats::Stats() {
	SetBackColor(Color::FromArgb(0, 0, 0, 0));

	// Font font = Font("DejaVu Sans Mono", 16, FontStyle::Bold);
	// Font font = Font("Courier New", 16, FontStyle::Bold);
	// Font font = Font("Consolas", 16, FontStyle::Bold);
	Font font = Font("Liberation Sans", 16, FontStyle::Bold);
	Color backColor = Color::FromArgb(0, 0, 0, 0);
	Color foreColor = Color::FromArgb(151, 151, 151);

#ifdef SHOW_HEARTBEAT
	m_heartbeat = new Heartbeat();
	m_heartbeat->SetDock(DockStyle::Left);
	m_heartbeat->SetBackColor(backColor);
	m_heartbeat->SetForeColor(foreColor);
	AddWidget(m_heartbeat);
#endif

#ifdef SHOW_SPINNER
	m_spinner = new Spinner();
	m_spinner->SetDock(DockStyle::Right);
	m_spinner->SetBackColor(backColor);
	m_spinner->SetForeColor(foreColor);
	AddWidget(m_spinner);
#endif

#ifdef SHOW_FPS
	m_fps = 0;
	m_fpsPreviousElapsedTime = 0;

	m_labelFPS = new Label();
	m_labelFPS->SetFont(font);
	m_labelFPS->SetDock(DockStyle::Right);
	m_labelFPS->SetTextAlign(ContentAlignment::MiddleRight);
	m_labelFPS->SetBackColor(backColor);
	m_labelFPS->SetForeColor(foreColor);
	m_labelFPS->SetWidth(120);
	AddWidget(m_labelFPS);

	m_fpsChronoLap.Start();
#endif

#ifdef SHOW_WIDGETS
	m_drawedControls = 0;
	m_labelControls = new Label();
	m_labelControls->SetFont(font);
	m_labelControls->SetDock(DockStyle::Right);
	m_labelControls->SetTextAlign(ContentAlignment::MiddleRight);
	m_labelControls->SetBackColor(backColor);
	m_labelControls->SetForeColor(foreColor);
	m_labelControls->SetWidth(120);
	AddWidget(m_labelControls);
#endif

	SetHeight(24);
}

Stats::~Stats() {
}

void Stats::SetTimeBeforeIddle() {
}

void Stats::SetTimeAfterIddle() {
#ifdef SHOW_FPS
	m_fps++;

	float elapsedTime = m_fpsChronoLap.GetTotalDuration();
	if ((elapsedTime - m_fpsPreviousElapsedTime) >= TimeToMeasure) {
		float fps = m_fps / (elapsedTime - m_fpsPreviousElapsedTime);
		m_labelFPS->SetText(String("%.0f FPS", Math::Round(fps)));
		m_fps = 0;
		m_fpsPreviousElapsedTime = elapsedTime;
	}
#endif
}

Stats *Stats::Instance() {
	if (Stats::m_instance == 0)
		Stats::m_instance = new Stats;

	return Stats::m_instance;
}

void Stats::OnRemoteHeartbeat() {
#ifdef SHOW_HEARTBEAT
	m_heartbeat->OnRemoteHeartbeat();
#endif
}

void Stats::SetDrawedControls(int drawedControls) {
#ifdef SHOW_WIDGETS
	if (m_drawedControls != drawedControls) {
		m_drawedControls = drawedControls;
		m_labelControls->SetText(String("%d widgets", m_drawedControls));
	}
#endif
}
