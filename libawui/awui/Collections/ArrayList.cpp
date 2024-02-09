/**
 * awui/Collections/ArrayList.cpp
 *
 * Copyright (C) 2011 Borja Sánchez Zamorano
 */

#include "ArrayList.h"

#include <awui/String.h>
#include <assert.h>
#include <stdlib.h>

using namespace awui::Collections;

ArrayList::ArrayList() {
	this->_count = 0;
	this->_size = 8;
	this->_data = (Object **) malloc (this->_size * sizeof(Object *));
}

ArrayList::~ArrayList() {
	free(this->_data);
}

bool ArrayList::IsClass(Classes objectClass) const {
	if (objectClass == Classes::ArrayList) {
		return true;
	}

	return Object::IsClass(objectClass);
}

awui::String ArrayList::ToString() {
	return "awui.Collections.ArrayList";
}

void ArrayList::Add(Object * item) {
	if (this->_count == this->_size) {
		this->_size = this->_size * 2;
		this->_data = (Object **) realloc (this->_data, this->_size * sizeof(Object *));
	}

	this->_data[this->_count] = item;
	this->_count++;
}

void ArrayList::Clear() {
	this->_count = 0;
	this->_size = 8;
	this->_data = (Object **) realloc (this->_data, this->_size * sizeof(Object *));
}

int ArrayList::IndexOf(Object * item) {
	for (int i = 0; i < this->_count; i++)
		if (this->_data[i] == item)
			return i;

	return -1;
}

awui::Object * ArrayList::Get(int index) {
	if (index < this->_count)
		return this->_data[index];

	assert(0);
	return NULL;
}

void ArrayList::Remove(Object * item) {
	int pos = this->IndexOf(item);

	if (pos != -1)
		this->RemoveAt(pos);
}

void ArrayList::RemoveAt(int index) {
	for (int i = index + 1; i < this->_count; i++)
		this->_data[i - 1] = this->_data[i];

	if (index < this->_count) {
		this->_count--;

		if ((this->_size > 8) && ((this->_size >> 1) > this->_count)) {
			this->_size = this->_size >> 1;
			this->_data = (Object **) realloc (this->_data, this->_size * sizeof(Object *));
		}
	}
}

void ArrayList::SetChildIndex(Object * item, int newIndex) {
	// newIndex no puede ser mayor que el tamaño
	if (newIndex >= this->_count)
		return;

	// Sino encontramos el item, no se mueve nada
	int oldIndex = this->IndexOf(item);
	if (oldIndex == -1)
		return;

	for (int i = oldIndex; i < newIndex; i++)
		this->_data[i] = this->_data[i + 1];

	for (int i = oldIndex; i > newIndex; i--)
		this->_data[i] = this->_data[i - 1];

	this->_data[newIndex] = item;
}

void ArrayList::Replace(Object * oldItem, Object * newItem) {
	if (oldItem && newItem) {
		int index = this->IndexOf(oldItem);

		if (index != -1)
			this->_data[index] = newItem;
		else
			this->Add(newItem);
	} else {
		if (oldItem)
			this->Remove(oldItem);
		if (newItem)
			this->Add(newItem);
	}
}
