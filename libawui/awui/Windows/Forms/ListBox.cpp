// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "ListBox.h"

#include <awui/Collections/ArrayList.h>

using namespace awui::Collections;
using namespace awui::Windows::Forms;

ListBox::ListBox() {
	this->collection = new ObjectCollection(this);
}

ListBox::~ListBox() {
	delete this->collection;
}

int ListBox::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::ListBox)
		return 1;

	return Control::IsClass(objectClass);
}

ObjectCollection * ListBox::GetItems() const {
	return this->collection;
}

ObjectCollection::ObjectCollection(ListBox * owner) : Object() {
	this->listbox = owner;
	this->arraylist = new ArrayList();
}

ObjectCollection::~ObjectCollection() {
	delete this->arraylist;
}

int ObjectCollection::GetCount() const {
	return this->arraylist->GetCount();
}

void ObjectCollection::Add(Object * item) {
	this->arraylist->Add(item);
}

void ObjectCollection::Clear() {
	this->arraylist->Clear();
}

bool ObjectCollection::Contains(Object * value) const {
	return (this->arraylist->IndexOf(value) != -1);
}

int ObjectCollection::IndexOf(Object * value) const {
	return this->arraylist->IndexOf(value);
}

void ObjectCollection::Remove(Object * value) {
	this->arraylist->Remove(value);
}

void ObjectCollection::RemoveAt(int index) {
	this->arraylist->RemoveAt(index);
}