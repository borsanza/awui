// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "ControlCollection.h"

#include <awui/Windows/Forms/Control.h>
#include <awui/Windows/Forms/Form.h>

using namespace awui::Collections;
using namespace awui::Windows::Forms;

ControlCollection::ControlCollection(Control * owner) {
	this->owner = owner;
}

ControlCollection::~ControlCollection() {
}

int ControlCollection::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::ControlCollection)
		return 1;

	return ArrayList::IsClass(objectClass);
}

Control * ControlCollection::GetOwner() {
	return this->owner;
}

#include <awui/Console.h>
#include <awui/Convert.h>

void ControlCollection::Add(Control * item) {
	ArrayList::Add(item);
	item->SetParent(this->owner);
	this->owner->Layout();
}
