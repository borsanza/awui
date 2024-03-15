#pragma once

namespace awui::Windows::Forms {
	struct JoystickButtons {
		enum Enum {
			// clang-format off
			JOYSTICK_BUTTON_A             = 2 << 0,
			JOYSTICK_BUTTON_B             = 2 << 1,
			JOYSTICK_BUTTON_X             = 2 << 2,
			JOYSTICK_BUTTON_Y             = 2 << 3,
			JOYSTICK_BUTTON_BACK          = 2 << 4,
			JOYSTICK_BUTTON_GUIDE         = 2 << 5,
			JOYSTICK_BUTTON_START         = 2 << 6,
			JOYSTICK_BUTTON_LEFTSTICK     = 2 << 7,
			JOYSTICK_BUTTON_RIGHTSTICK    = 2 << 8,
			JOYSTICK_BUTTON_LEFTSHOULDER  = 2 << 9,
			JOYSTICK_BUTTON_RIGHTSHOULDER = 2 << 10,
			JOYSTICK_BUTTON_DPAD_UP       = 2 << 11,
			JOYSTICK_BUTTON_DPAD_DOWN     = 2 << 12,
			JOYSTICK_BUTTON_DPAD_LEFT     = 2 << 13,
			JOYSTICK_BUTTON_DPAD_RIGHT    = 2 << 14,
			JOYSTICK_BUTTON_MISC1         = 2 << 15,
			JOYSTICK_BUTTON_PADDLE1       = 2 << 16,
			JOYSTICK_BUTTON_PADDLE2       = 2 << 17,
			JOYSTICK_BUTTON_PADDLE3       = 2 << 18,
			JOYSTICK_BUTTON_PADDLE4       = 2 << 19,
			JOYSTICK_BUTTON_TOUCHPAD      = 2 << 20,
			// clang-format on
		};
	};
} // namespace awui::Windows::Forms
