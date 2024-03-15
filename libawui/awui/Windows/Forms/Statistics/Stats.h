#pragma once

#include <awui/DateTime.h>
#include <awui/TimeSpan.h>
#include <awui/Windows/Forms/Label.h>
#include <awui/Windows/Forms/Panel.h>
#include <awui/Windows/Forms/Statistics/Heartbeat.h>
#include <awui/Windows/Forms/Statistics/Spinner.h>

#define SHOW_PERCENT 1
#define SHOW_SPINNER 1
#define SHOW_WIDGETS 1
#define SHOW_FPS 1

// clang-format off
#ifndef NDEBUG
	#define SHOW_WIDGETS 1
	#define SHOW_FPS 1
#endif
// clang-format on

#define SHOW_HEARTBEAT 1

namespace awui::Windows::Forms::Statistics {
	class Stats : public Panel {
	  private:
		static Stats *_instance;

		DateTime _afterSync;
		DateTime _beforeSync;
		TimeSpan _idle;

#if defined(SHOW_FPS) || defined(SHOW_PERCENT)
		int _lastSecond;
		DateTime _lastTime;
#endif

#ifdef SHOW_WIDGETS
		Label *_labelControls;
		int _drawedControls;
#endif

#ifdef SHOW_HEARTBEAT
		Heartbeat *_heartbeat;
#endif

#ifdef SHOW_FPS
		int _fps;
		float _fpsCalculated;
		Label *_labelFPS;
#endif

#ifdef SHOW_SPINNER
		Spinner *_spinner;
#endif

#ifdef SHOW_PERCENT
		long long _timeUsed;
		Label *_labelPercent;
		float _percent;
#endif

		Stats();
		virtual ~Stats();

	  public:
		static Stats *Instance();

		void SetTimeBeforeIddle();
		void SetTimeAfterIddle();

		virtual void OnRemoteHeartbeat();
		void SetDrawedControls(int drawedControls);

		TimeSpan GetIdle();
	};
} // namespace awui::Windows::Forms::Statistics
