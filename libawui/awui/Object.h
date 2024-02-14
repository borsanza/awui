#pragma once

#include <cstdint>

namespace awui {
	class String;

	enum class Classes {
		Object,
		Application,
		ArrayList,
		SortedList,
		Button,
		Bitmap,
		ImageFader,
		Label,
		Keyboard,
		Color,
		ColorF,
		Control,
		ControlCollection,
		Form,
		Graphics,
		Image,
		MouseEventArgs,
		JoystickEventArgs,
		JoystickMoveEventArgs,
		Panel,
		Pen,
		SplitContainer,
		Splitter,
		ListBox,
		Stack,
		Chip8,
		MasterSystem,
		DebuggerSMS,
		Spectrum,
		Gradient,
		MenuButton,
		Shader,
		ArcadeContainer,
	};

	class Object {
		public:
			Object();
			virtual ~Object();

			virtual bool IsClass(Classes objectClass) const;

			virtual String ToString();
	};
}
