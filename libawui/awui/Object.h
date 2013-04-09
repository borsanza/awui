// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUIOBJECT_H__
#define __AWUIOBJECT_H__

namespace awui {
	enum Classes {
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

	class Object {
	public:
		Object();
		~Object();

		virtual int IsClass(awui::Classes objectClass) const;
	};
}

#endif
