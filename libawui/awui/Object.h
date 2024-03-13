#pragma once

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
		Form,
		Graphics,
		Image,
		EventArgs,
		MouseEventArgs,
		JoystickEventArgs,
		JoystickMoveEventArgs,
		JoystickButtonEventArgs,
		JoystickAxisMotionEventArgs,
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
		ConfigButton,
		SettingsUI
	};

	class Object {
		protected:
			Classes m_class;

		public:
			Object();
			virtual ~Object() = default;

			virtual bool IsClass(Classes objectClass) const;

			virtual String ToString() const;
	};
}
