// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiControlCollection.h"
#include "awuiControl.h"

awuiControlCollection::awuiControlCollection(awuiControl * owner) {
	this->owner = owner;
}

awuiControlCollection::~awuiControlCollection() {
}

awuiControl * awuiControlCollection::GetOwner() {
	return this->owner;
}

void awuiControlCollection::Add(void * item) {
	awuiArrayList::Add(item);
	this->owner->Layout();
}