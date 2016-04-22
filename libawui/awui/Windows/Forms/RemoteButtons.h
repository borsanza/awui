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

					SNES_SELECT = 0x0040,
					SNES_START  = 0x0080,
					SNES_A = 0x0200,
					SNES_B = 0x0020,
					SNES_X = 0x0100,
					SNES_Y = 0x0010,
					SNES_L = 0x0400,
					SNES_R = 0x0800,
					
/*
Select+Start              Exit
Select+X                  RGUI Menu
// Select+Right Shoulder  Save
// Select+Left Shoulder   Load
// Select+Right           Input State Slot Increase
// Select+Left            Input State Slot Decrease
// Select+B               Reset
*/

					SPECIAL_RESET = SNES_SELECT | SNES_B,
					SPECIAL_SLOT_INCREASE = SNES_SELECT | Right,
					SPECIAL_SLOT_DECREASE = SNES_SELECT | Left,
					SPECIAL_LOAD = SNES_SELECT | SNES_L,
					SPECIAL_SAVE = SNES_SELECT | SNES_R,
				};
			};
		}
	}
}

#endif
