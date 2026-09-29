// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "ListBox.h"

#include <algorithm>

using namespace awui::Windows::Forms;

ListBox::ListBox() {
	m_collection = new ObjectCollection(this);
}

ListBox::~ListBox() {
	delete m_collection;
}

ObjectCollection *ListBox::GetItems() const {
	return m_collection;
}

ObjectCollection::ObjectCollection(ListBox *owner) : Object() {
	listbox = owner;
}

ObjectCollection::~ObjectCollection() {
}

int ObjectCollection::GetCount() const {
	return (int) m_items.size();
}

void ObjectCollection::Add(Object *item) {
	m_items.push_back(item);
}

void ObjectCollection::Clear() {
	m_items.clear();
}

bool ObjectCollection::Contains(Object *value) const {
	return IndexOf(value) != -1;
}

int ObjectCollection::IndexOf(Object *value) const {
	auto it = std::find(m_items.begin(), m_items.end(), value);
	return (it != m_items.end()) ? (int) (it - m_items.begin()) : -1;
}

void ObjectCollection::Remove(Object *value) {
	m_items.erase(std::remove(m_items.begin(), m_items.end(), value), m_items.end());
}

void ObjectCollection::RemoveAt(int index) {
	if ((index >= 0) && (index < (int) m_items.size()))
		m_items.erase(m_items.begin() + index);
}