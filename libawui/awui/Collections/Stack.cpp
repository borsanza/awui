/*
 * awui/Collections/Stack.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "Stack.h"

#include <awui/String.h>
#include <stdlib.h>

using namespace awui::Collections;

Stack::Stack() {
	this->_last = NULL;
	this->_count = 0;
}

Stack::~Stack() {
	this->Clear();
}

int Stack::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Stack)
		return 1;

	return Object::IsClass(objectClass);
}

awui::String Stack::ToString() {
	return "awui.Collections.Stack";
}

void Stack::Push(Object * item) {
	StackItem * itemList = (StackItem *) malloc(sizeof (struct StackItem));
	itemList->object = item;
	itemList->prev = this->_last;
	this->_count++;
	this->_last = itemList;
}

int Stack::GetCount() {
	return this->_count;
}

void Stack::Clear() {
	while (this->GetCount() > 0)
		this->Pop();
}

awui::Object * Stack::Pop() {
	if (!this->_last)
		return NULL;

	Object * r = this->_last->object;

	StackItem * prev = this->_last->prev;

	free(this->_last);
	this->_count--;

	this->_last = prev;

	return r;
}