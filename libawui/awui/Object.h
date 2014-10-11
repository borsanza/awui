// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIOBJECT_H__
#define __AWUIOBJECT_H__

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

#endif