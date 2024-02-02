#pragma once

#include <cstdint>

namespace awui {
	class String;

	struct Classes {
		enum Enum {
			Object,
			Application,
			ArrayList,
			SortedList,
			Button,
			Bitmap,
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
		};
	};

	class Object {
		public:
			Object();
			virtual ~Object();

			virtual int IsClass(Classes::Enum objectClass) const;

			virtual String ToString();
	};
}
