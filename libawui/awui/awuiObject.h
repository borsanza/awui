// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUIOBJECT_H__
#define __AWUIOBJECT_H__

class awuiObject {
public:
	enum awuiClasses {
		Object,
		Application,
		ArrayList,
		Bitmap,
		Button,
		Color,
		Component,
		Control,
		ControlCollection,
		Form,
		Graphics,
		Image,
		MouseEventArgs,
		Panel,
		Pen,
		SplitContainer,
		Splitter
	};
	
	awuiObject();
	~awuiObject();
	
	virtual int IsClass(awuiClasses objectClass);
};

#endif