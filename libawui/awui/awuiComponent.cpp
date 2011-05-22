// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiComponent.h"

awuiComponent::awuiComponent() {
}

awuiComponent::~awuiComponent() {
}

int awuiComponent::IsClass(awuiObject::awuiClasses objectClass) {
	if (objectClass == awuiObject::Component)
		return 1;

	return awuiObject::IsClass(objectClass);
}