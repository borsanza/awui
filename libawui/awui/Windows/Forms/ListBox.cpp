// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "ListBox.h"

#include <awui/Collections/ArrayList.h>

using namespace awui::Collections;
using namespace awui::Windows::Forms;

ListBox::ListBox() {
	m_class = Classes::ListBox;
	m_collection = new ObjectCollection(this);
}

ListBox::~ListBox() {
	delete m_collection;
}

bool ListBox::IsClass(Classes objectClass) const {
	return (objectClass == Classes::ListBox) || Control::IsClass(objectClass);
}

ObjectCollection * ListBox::GetItems() const {
	return m_collection;
}

ObjectCollection::ObjectCollection(ListBox * owner) : Object() {
	listbox = owner;
	arraylist = new ArrayList();
}

ObjectCollection::~ObjectCollection() {
	delete arraylist;
}

int ObjectCollection::GetCount() const {
	return arraylist->GetCount();
}

void ObjectCollection::Add(Object * item) {
	arraylist->Add(item);
}

void ObjectCollection::Clear() {
	arraylist->Clear();
}

bool ObjectCollection::Contains(Object * value) const {
	return (arraylist->IndexOf(value) != -1);
}

int ObjectCollection::IndexOf(Object * value) const {
	return arraylist->IndexOf(value);
}

void ObjectCollection::Remove(Object * value) {
	arraylist->Remove(value);
}

void ObjectCollection::RemoveAt(int index) {
	arraylist->RemoveAt(index);
}