// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "ControlCollection.h"

#include <awui/Windows/Forms/Control.h>

using namespace awui::Collections;
using namespace awui::Windows::Forms;

ControlCollection::ControlCollection(Control * owner) {
	this->owner = owner;
}

ControlCollection::~ControlCollection() {
}

int ControlCollection::IsClass(Classes objectClass) const {
	if (objectClass == awui::ControlCollection)
		return 1;

	return ArrayList::IsClass(objectClass);
}

Control * ControlCollection::GetOwner() {
	return this->owner;
}

void ControlCollection::Add(void * item) {
	ArrayList::Add(item);
	((Control *)item)->SetParent(this->owner);
	this->owner->Layout();
}
