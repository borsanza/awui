// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Stats.h"

#include <awui/Drawing/Font.h>
#include <awui/Math.h>
#include <awui/UI/Diagnostics/Heartbeat.h>
#include <awui/UI/Diagnostics/Spinner.h>

using namespace awui::Drawing;
using namespace awui::UI::Diagnostics;

Stats::Stats() {
	SetBackColor(Color::Transparent);

	Font font = Font("Liberation Sans", 16, FontStyle::Bold);
	Color foreColor = Color::FromArgb(151, 151, 151);

	m_heartbeat = new Heartbeat();
	m_heartbeat->SetDock(DockStyle::Left);
	m_heartbeat->SetBackColor(Color::Transparent);
	m_heartbeat->SetForeColor(foreColor);
	AddWidget(m_heartbeat);

	m_spinner = new Spinner();
	m_spinner->SetDock(DockStyle::Right);
	m_spinner->SetBackColor(Color::Transparent);
	m_spinner->SetForeColor(foreColor);
	AddWidget(m_spinner);

	m_frames = 0;
	m_fpsPreviousElapsedTime = 0;
	m_labelFps = new Label();
	m_labelFps->SetFont(font);
	m_labelFps->SetDock(DockStyle::Right);
	m_labelFps->SetTextAlign(ContentAlignment::MiddleRight);
	m_labelFps->SetBackColor(Color::Transparent);
	m_labelFps->SetForeColor(foreColor);
	m_labelFps->SetWidth(120);
	AddWidget(m_labelFps);
	m_fpsChronoLap.Start();

	m_drawedControls = 0;
	m_labelWidgets = new Label();
	m_labelWidgets->SetFont(font);
	m_labelWidgets->SetDock(DockStyle::Right);
	m_labelWidgets->SetTextAlign(ContentAlignment::MiddleRight);
	m_labelWidgets->SetBackColor(Color::Transparent);
	m_labelWidgets->SetForeColor(foreColor);
	m_labelWidgets->SetWidth(120);
	AddWidget(m_labelWidgets);

	SetHeight(24);

	// Como estaba antes con los #define: solo los FPS
	SetShowFps(true);
	SetShowWidgetCount(false);
	SetShowHeartbeat(false);
	SetShowSpinner(false);
}

// Los indicadores ocultos no ocupan sitio: se vuelve a repartir la barra
void Stats::ShowIndicator(Control *indicator, bool show) {
	indicator->SetVisible(show);
	Layout();
}

void Stats::SetShowFps(bool show) {
	ShowIndicator(m_labelFps, show);
}

void Stats::SetShowWidgetCount(bool show) {
	ShowIndicator(m_labelWidgets, show);
	if (show)
		m_labelWidgets->SetText(String("%d widgets", m_drawedControls));
}

void Stats::SetShowHeartbeat(bool show) {
	ShowIndicator(m_heartbeat, show);
}

void Stats::SetShowSpinner(bool show) {
	ShowIndicator(m_spinner, show);
}

void Stats::SetTimeBeforeIddle() {
}

void Stats::SetTimeAfterIddle() {
	m_frames++;
	float elapsedTime = m_fpsChronoLap.GetTotalDuration();
	if ((elapsedTime - m_fpsPreviousElapsedTime) >= TimeToMeasure) {
		float fps = m_frames / (elapsedTime - m_fpsPreviousElapsedTime);
		// Solo se vuelve a dibujar el texto si se ve
		if (m_labelFps->GetVisible())
			m_labelFps->SetText(String("%.0f FPS", Math::Round(fps)));
		m_frames = 0;
		m_fpsPreviousElapsedTime = elapsedTime;
	}
}

void Stats::OnRemoteHeartbeat() {
	m_heartbeat->OnRemoteHeartbeat();
}

void Stats::SetDrawedControls(int drawedControls) {
	if (m_drawedControls != drawedControls) {
		m_drawedControls = drawedControls;
		if (m_labelWidgets->GetVisible())
			m_labelWidgets->SetText(String("%d widgets", m_drawedControls));
	}
}
