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
	m_class = Classes::ArrayList;
	m_count = 0;
	m_size = 8;
	m_data = (Object **) malloc (m_size * sizeof(Object *));
}

ArrayList::~ArrayList() {
	free(m_data);
}

bool ArrayList::IsClass(Classes objectClass) const {
	return (objectClass == Classes::ArrayList) || Object::IsClass(objectClass);
}

awui::String ArrayList::ToString() const {
	return "awui.Collections.ArrayList";
}

void ArrayList::Add(Object * item) {
	if (m_count == m_size) {
		m_size = m_size * 2;
		m_data = (Object **) realloc (m_data, m_size * sizeof(Object *));
	}

	m_data[m_count] = item;
	m_count++;
}

void ArrayList::Clear() {
	m_count = 0;
	m_size = 8;
	m_data = (Object **) realloc (m_data, m_size * sizeof(Object *));
}

int ArrayList::IndexOf(Object * item) const {
	for (int i = 0; i < m_count; i++)
		if (m_data[i] == item)
			return i;

	return -1;
}

awui::Object * ArrayList::Get(int index) const {
	if ((index < 0) || (index >= m_count)) {
		assert(0);
		return nullptr;
	}

	return m_data[index];
}

void ArrayList::Remove(Object * item) {
	int pos = IndexOf(item);

	if (pos != -1)
		RemoveAt(pos);
}

void ArrayList::RemoveAt(int index) {
	for (int i = index + 1; i < m_count; i++)
		m_data[i - 1] = m_data[i];

	if (index < m_count) {
		m_count--;

		if ((m_size > 8) && ((m_size >> 1) > m_count)) {
			m_size = m_size >> 1;
			m_data = (Object **) realloc (m_data, m_size * sizeof(Object *));
		}
	}
}

void ArrayList::SetChildIndex(Object * item, int newIndex) {
	// newIndex no puede ser mayor que el tamaño
	if (newIndex >= m_count)
		return;

	// Sino encontramos el item, no se mueve nada
	int oldIndex = IndexOf(item);
	if (oldIndex == -1)
		return;

	for (int i = oldIndex; i < newIndex; i++)
		m_data[i] = m_data[i + 1];

	for (int i = oldIndex; i > newIndex; i--)
		m_data[i] = m_data[i - 1];

	m_data[newIndex] = item;
}

void ArrayList::Replace(Object * oldItem, Object * newItem) {
	if (oldItem && newItem) {
		int index = IndexOf(oldItem);

		if (index != -1)
			m_data[index] = newItem;
		else
			Add(newItem);
	} else {
		if (oldItem)
			Remove(oldItem);
		if (newItem)
			Add(newItem);
	}
}
