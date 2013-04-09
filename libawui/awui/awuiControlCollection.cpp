// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiControlCollection.h"

#include <awui/Windows/Forms/Control.h>

using namespace awui;
using namespace awui::Collections;
using namespace awui::Windows;

awuiControlCollection::awuiControlCollection(Forms::Control * owner) {
	this->owner = owner;
}

awuiControlCollection::~awuiControlCollection() {
}

int awuiControlCollection::IsClass(Classes objectClass) const {
	if (objectClass == awui::ControlCollection)
		return 1;

	return ArrayList::IsClass(objectClass);
}

Forms::Control * awuiControlCollection::GetOwner() {
	return this->owner;
}

void awuiControlCollection::Add(void * item) {
	ArrayList::Add(item);
	((Forms::Control *)item)->SetParent(this->owner);
	this->owner->Layout();
}
