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

bool ControlCollection::IsClass(Classes objectClass) const {
	if (objectClass == Classes::ControlCollection) {
		return true;
	}

	return ArrayList::IsClass(objectClass);
}

Control * ControlCollection::GetOwner() {
	return this->owner;
}

void ControlCollection::Add(Control * item, bool fixSelected) {
	ArrayList::Add(item);
	item->SetParent(owner);
	owner->Layout();

	if (fixSelected && item->GetFocused())
		item->SetFocus(false);
}

void ControlCollection::Remove(Control * item) {
	item->CheckMouseControl();

	ArrayList::Remove(item);
	item->SetParent(NULL);
	this->owner->Layout();
}

void ControlCollection::MoveToEnd(Control * item) {
	this->Remove(item);
	this->Add(item);
}

void ControlCollection::Replace(Object * oldItem, Object * newItem) {
	Control* oldControl = dynamic_cast<Control*>(oldItem);
    Control* newControl = dynamic_cast<Control*>(newItem);

	if (oldControl)
		oldControl->CheckMouseControl();

	ArrayList::Replace(oldControl, newControl);

	if (newControl)
		newControl->SetParent(this->owner);
}
