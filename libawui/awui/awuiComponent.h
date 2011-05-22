// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUICOMPONENT_H__
#define __AWUICOMPONENT_H__

#include "awuiObject.h"

class awuiComponent : public awuiObject {
public:
	awuiComponent();
	~awuiComponent();

	virtual int IsClass(awuiClasses objectClass);
};

#endif