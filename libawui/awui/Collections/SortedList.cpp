/**
 * awui/Collections/SortedList.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "SortedList.h"

#include <awui/String.h>
#include <stdlib.h>

using namespace awui::Collections;

SortedList::SortedList() {
	m_class = Classes::SortedList;

	m_first = NULL;
	m_last = NULL;
	m_count = 0;
}

SortedList::~SortedList() {
	Clear();
}

bool SortedList::IsClass(Classes objectClass) const {
	return (objectClass == Classes::SortedList) || Object::IsClass(objectClass);
}

awui::String SortedList::ToString() const {
	return "awui.Collections.SortedList";
}

void SortedList::Add(Object *key, Object *value) {
	SortedListItem *itemList = (SortedListItem *) malloc(sizeof(struct SortedListItem));
	itemList->key = key;
	itemList->value = value;
	itemList->next = NULL;
	m_count++;

	if (m_first == NULL) {
		m_first = itemList;
		m_last = itemList;
		return;
	}

	// Para acelerar inserciones, miramos si lo podemos insertar en ultima posicion
	// Este algoritmo es super optimizable, pero para salir del paso lo dejo asi
	if (key->ToString() >= m_last->key->ToString()) {
		m_last->next = itemList;
		m_last = itemList;
		return;
	}

	SortedListItem *itemListAux = m_first;
	while (itemListAux->next != NULL) {
		if (key->ToString() < itemListAux->key->ToString())
			break;

		itemListAux = itemListAux->next;
	}

	itemList->key = itemListAux->key;
	itemList->value = itemListAux->value;
	itemListAux->key = key;
	itemListAux->value = value;

	itemList->next = itemListAux->next;
	itemListAux->next = itemList;

	while (m_last->next)
		m_last = m_last->next;
}

int SortedList::GetCount() {
	return m_count;
}

void SortedList::Clear() {
	while (GetCount() > 0)
		RemoveAt(0);
}

awui::Object *SortedList::GetKey(int index) {
	int pos = 0;

	SortedListItem *itemListAux = m_first;

	while (itemListAux != NULL) {
		if (pos == index)
			return itemListAux->key;

		pos++;
		itemListAux = itemListAux->next;
	}

	return NULL;
}

awui::Object *SortedList::GetByIndex(int index) {
	int pos = 0;

	SortedListItem *itemListAux = m_first;

	while (itemListAux != NULL) {
		if (pos == index)
			return itemListAux->value;

		pos++;
		itemListAux = itemListAux->next;
	}

	return NULL;
}

void SortedList::RemoveAt(int index) {
	int pos = 0;
	SortedListItem *itemListAux = m_first;
	SortedListItem *last = NULL;

	while (itemListAux != NULL) {
		if (pos == index) {
			m_count--;

			if (last == NULL)
				m_first = itemListAux->next;
			else
				last->next = itemListAux->next;

			free(itemListAux);
			return;
		}

		pos++;
		last = itemListAux;
		itemListAux = itemListAux->next;
	}
}
