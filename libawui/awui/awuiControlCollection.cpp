// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiControlCollection.h"

#include <awui/awuiControl.h>

using namespace awui::Collections;

awuiControlCollection::awuiControlCollection(awuiControl * owner) {
	this->owner = owner;
}

awuiControlCollection::~awuiControlCollection() {
}

int awuiControlCollection::IsClass(Classes objectClass) const {
	if (objectClass == awui::ControlCollection)
		return 1;

	return ArrayList::IsClass(objectClass);
}

awuiControl * awuiControlCollection::GetOwner() {
	return this->owner;
}

void awuiControlCollection::Add(void * item) {
	ArrayList::Add(item);
	((awuiControl *)item)->SetParent(this->owner);
	this->owner->Layout();
}
