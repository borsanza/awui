#ifndef _AWUI_WINDOWS_FORMS_REMOTEBUTTONS_H
#define _AWUI_WINDOWS_FORMS_REMOTEBUTTONS_H

namespace awui {
	namespace Windows {
		namespace Forms {
			struct RemoteButtons {
				enum Enum {
					None    = 0x0000,
					Up      = 0x0001,
					Down    = 0x0002,
					Left    = 0x0004,
					Right   = 0x0008,
					Ok      = 0x0010,
					Play    = 0x0020,
					Menu    = 0x0040,
					Pause   = 0x0080,
					Button3 = 0x0100,
					Button4 = 0x0200,
					Button5 = 0x0400,
					Button6 = 0x0800,
					Button7 = 0x1000,
					Button8 = 0x2000,
				};
			};
		}
	}
}

#endif
