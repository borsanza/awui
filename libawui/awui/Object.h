// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIOBJECT_H__
#define __AWUIOBJECT_H__

namespace awui {
	struct Classes {
		enum Enum {
			Object,
			Application,
			ArrayList,
			Button,
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
		};
	};

	class Object {
	public:
		Object();
		virtual ~Object();

		virtual int IsClass(Classes::Enum objectClass) const;
	};
}

#endif
