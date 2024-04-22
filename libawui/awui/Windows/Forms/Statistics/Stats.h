#pragma once

#include <awui/ChronoLap.h>
#include <awui/Windows/Forms/Label.h>
#include <awui/Windows/Forms/Panel.h>
#include <awui/Windows/Forms/Statistics/Heartbeat.h>
#include <awui/Windows/Forms/Statistics/Spinner.h>

// #define SHOW_SPINNER
#define SHOW_FPS
// #define SHOW_WIDGETS
//  #define SHOW_HEARTBEAT

const float TimeToMeasure = 1.0f;

namespace awui::Windows::Forms::Statistics {
	class Stats : public Panel {
	  private:
		static Stats *m_instance;

#ifdef SHOW_WIDGETS
		Label *m_labelControls;
		int m_drawedControls;
#endif

#ifdef SHOW_HEARTBEAT
		Heartbeat *m_heartbeat;
#endif

#ifdef SHOW_FPS
		ChronoLap m_fpsChronoLap;
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
} // namespace awui::Windows::Forms::Statistics
