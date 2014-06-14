#ifndef _AWUI_WINDOWS_FORMS_REMOTEBUTTONS_H
#define _AWUI_WINDOWS_FORMS_REMOTEBUTTONS_H

namespace awui {
	namespace Windows {
		namespace Forms {
			struct RemoteButtons {
				enum Enum {
					None = 0,
					Menu = 1,
					Ok = 2,
					Play = 4,
					Right = 8,
					Left = 16,
					Up = 32,
					Down = 64,
				};
			};
		}
	}
}

#endif