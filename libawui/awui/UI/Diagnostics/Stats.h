#pragma once

#include <awui/Time/ChronoLap.h>
#include <awui/UI/Label.h>
#include <awui/UI/Panel.h>
#include <awui/UI/Diagnostics/Heartbeat.h>
#include <awui/UI/Diagnostics/Spinner.h>

// #define SHOW_SPINNER
#define SHOW_FPS
// #define SHOW_WIDGETS
//  #define SHOW_HEARTBEAT

const float TimeToMeasure = 1.0f;

namespace awui::UI::Diagnostics {
	class Stats : public Panel {
	  private:
		static Stats *s_instance;

#ifdef SHOW_WIDGETS
		Label *m_labelControls;
		int m_drawedControls;
#endif

#ifdef SHOW_HEARTBEAT
		Heartbeat *m_heartbeat;
#endif

#ifdef SHOW_FPS
		Time::ChronoLap m_fpsChronoLap;
		int m_fps;
		float m_fpsPreviousElapsedTime;
		Label *m_labelFPS;
#endif

#ifdef SHOW_SPINNER
		Spinner *m_spinner;
#endif

		Stats();
		virtual ~Stats();

	  public:
		static Stats *Instance();

		void SetTimeBeforeIddle();
		void SetTimeAfterIddle();

		virtual void OnRemoteHeartbeat();
		void SetDrawedControls(int drawedControls);
	};
} // namespace awui::UI::Diagnostics
