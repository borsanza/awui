// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/DateTime.h>
#include <awui/TimeSpan.h>
#include <awui/Windows/Forms/Label.h>
#include <awui/Windows/Forms/Panel.h>
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
						DateTime beforeSync_last;

						TimeSpan idle;
						TimeSpan total;
						TimeSpan used;

						Label labelidle;
						Label labelused;
						Label labeltotal;
						Spinner spinner;

						Stats();
						virtual ~Stats();

					public:
						static Stats* Instance();

						void SetTimeBeforeVSync();
						void SetTimeAfterVSync();

						virtual void OnTick();
						TimeSpan GetIdle();
				};
			}
		}
	}
}
