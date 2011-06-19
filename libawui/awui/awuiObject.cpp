// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiObject.h"

awuiObject::awuiObject() {
}

awuiObject::~awuiObject() {
}

int awuiObject::IsClass(awuiObject::awuiClasses objectClass) const {
	if (objectClass == awuiObject::Object)
		return 1;

	return 0;
}