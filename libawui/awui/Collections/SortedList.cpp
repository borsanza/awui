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
	this->first = NULL;
	this->last = NULL;
	this->count = 0;
}

SortedList::~SortedList() {
	this->Clear();
}

bool SortedList::IsClass(Classes objectClass) const {
	if (objectClass == Classes::SortedList) {
		return true;
	}

	return Object::IsClass(objectClass);
}

awui::String SortedList::ToString() const {
	return "awui.Collections.SortedList";
}

void SortedList::Add(Object * key, Object * value) {
	SortedListItem * itemList = (SortedListItem *) malloc(sizeof (struct SortedListItem));
	itemList->key = key;
	itemList->value = value;
	itemList->next = NULL;
	this->count++;

	if (this->first == NULL) {
		this->first = itemList;
		this->last = itemList;
		return;
	}

	// Para acelerar inserciones, miramos si lo podemos insertar en ultima posicion
	// Este algoritmo es super optimizable, pero para salir del paso lo dejo asi
	if (key->ToString() >= this->last->key->ToString()) {
		this->last->next = itemList;
		this->last = itemList;
		return;
	}

	SortedListItem * itemListAux = this->first;
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

	while (this->last->next) this->last = this->last->next;
}

int SortedList::GetCount() {
	return this->count;
}

void SortedList::Clear() {
	while (this->GetCount() > 0)
		this->RemoveAt(0);
}

awui::Object * SortedList::GetKey(int index) {
	int pos = 0;

	SortedListItem * itemListAux = this->first;

	while (itemListAux != NULL) {
		if (pos == index)
			return itemListAux->key;

		pos++;
		itemListAux = itemListAux->next;
	}

	return NULL;
}

awui::Object * SortedList::GetByIndex(int index) {
	int pos = 0;

	SortedListItem * itemListAux = this->first;

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
	SortedListItem * itemListAux = this->first;
	SortedListItem * last = NULL;

	while (itemListAux != NULL) {
		if (pos == index) {
			this->count--;

			if (last == NULL)
				this->first = itemListAux->next;
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
