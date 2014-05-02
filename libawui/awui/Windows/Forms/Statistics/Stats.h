// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/DateTime.h>
#include <awui/TimeSpan.h>
#include <awui/Windows/Forms/Label.h>
#include <awui/Windows/Forms/Panel.h>
#include <awui/Windows/Forms/Statistics/Heartbeat.h>
#include <awui/Windows/Forms/Statistics/Spinner.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			namespace Statistics {
				class Stats : public Panel {
					private:
						static Stats* pinstance;

						DateTime afterSync;
						DateTime beforeSync;

						TimeSpan idle;

						Label *labelPercent;
						Label *labelControls;
						Label *labelFPS;
						Spinner *spinner;
						Heartbeat *heartbeat;

						DateTime lastTime;
						long long timeUsed;
						int lastSecond;
						int fps;
						int drawedControls;
						float fpsCalculated;
						float percent;

						Stats();
						virtual ~Stats();

					public:
						static Stats* Instance();

						void SetTimeBeforeVSync();
						void SetTimeAfterVSync();

						virtual void OnRemoteHeartbeat();
						void SetDrawedControls(int drawedControls);

						TimeSpan GetIdle();
				};
			}
		}
	}
}
