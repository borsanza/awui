#pragma once

#include <awui/DateTime.h>
#include <awui/TimeSpan.h>
#include <awui/Windows/Forms/Label.h>
#include <awui/Windows/Forms/Panel.h>
#include <awui/Windows/Forms/Statistics/Heartbeat.h>
#include <awui/Windows/Forms/Statistics/Spinner.h>

//#define SHOW_PERCENT 1
//#define SHOW_SPINNER 1

#ifndef NDEBUG
	#define SHOW_FPS 1
	#define SHOW_WIDGETS 1
#endif

#define SHOW_HEARTBEAT 1

namespace awui {
	namespace Windows {
		namespace Forms {
			namespace Statistics {
				class Stats : public Panel {
					private:
						static Stats* _instance;

						DateTime _afterSync;
						DateTime _beforeSync;
						TimeSpan _idle;

#if defined(SHOW_FPS) || defined(SHOW_PERCENT)
						int _lastSecond;
						DateTime _lastTime;
#endif // defined

#ifdef SHOW_WIDGETS
						Label * _labelControls;
						int _drawedControls;
#endif // SHOW_WIDGETS

#ifdef SHOW_HEARTBEAT
						Heartbeat * _heartbeat;
#endif // SHOW_HEARTBEAT

#ifdef SHOW_FPS
						int _fps;
						float _fpsCalculated;
						Label * _labelFPS;
#endif // SHOW_FPS

#ifdef SHOW_SPINNER
						Spinner * _spinner;
#endif // SHOW_SPINNER

#ifdef SHOW_PERCENT
						long long _timeUsed;
						Label * _labelPercent;
						float _percent;
#endif // SHOW_PERCENT

						Stats();
						virtual ~Stats();

					public:
						static Stats* Instance();

						void SetTimeBeforeIddle();
						void SetTimeAfterIddle();

						virtual void OnRemoteHeartbeat();
						void SetDrawedControls(int drawedControls);

						TimeSpan GetIdle();
				};
			}
		}
	}
}
