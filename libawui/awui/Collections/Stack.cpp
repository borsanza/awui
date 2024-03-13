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
	m_class = Classes::Stack;
	m_last = NULL;
	m_count = 0;
}

Stack::~Stack() {
	Clear();
}

bool Stack::IsClass(Classes objectClass) const {
	return (objectClass == Classes::Stack) || Object::IsClass(objectClass);
}

awui::String Stack::ToString() const {
	return "awui.Collections.Stack";
}

void Stack::Push(Object * item) {
	StackItem * itemList = (StackItem *) malloc(sizeof (struct StackItem));
	itemList->object = item;
	itemList->prev = m_last;
	m_count++;
	m_last = itemList;
}

int Stack::GetCount() {
	return m_count;
}

void Stack::Clear() {
	while (GetCount() > 0)
		Pop();
}

awui::Object * Stack::Pop() {
	if (!m_last)
		return NULL;

	Object * r = m_last->object;

	StackItem * prev = m_last->prev;

	free(m_last);
	m_count--;

	m_last = prev;

	return r;
}