// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiControlCollection.h"
#include "awuiControl.h"

awuiControlCollection::awuiControlCollection(awuiControl * owner) {
	this->owner = owner;
}

awuiControlCollection::~awuiControlCollection() {
}

int awuiControlCollection::IsClass(awuiObject::awuiClasses objectClass) {
	if (objectClass == awuiObject::ControlCollection)
		return 1;

	return awuiArrayList::IsClass(objectClass);
}

awuiControl * awuiControlCollection::GetOwner() {
	return this->owner;
}

void awuiControlCollection::Add(void * item) {
	awuiArrayList::Add(item);
	((awuiControl *)item)->SetParent(this->owner);
	this->owner->Layout();
}