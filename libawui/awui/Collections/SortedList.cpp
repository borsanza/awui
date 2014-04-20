// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "SortedList.h"

#include <awui/String.h>
#include <stdlib.h>

using namespace awui::Collections;

SortedList::SortedList() {
	this->first = NULL;
	this->count = 0;
}

SortedList::~SortedList() {
	this->Clear();
}

int SortedList::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::SortedList)
		return 1;

	return Object::IsClass(objectClass);
}

awui::String SortedList::ToString() {
	return "awui.Collections.SortedList";
}

void SortedList::Add(Object * key, Object * value) {
	ArraListItem * itemList = (ArraListItem *) malloc(sizeof (struct ArraListItem));
	itemList->key = key;
	itemList->value = value;
	itemList->next = NULL;
	this->count++;

	if (this->first == NULL) {
		this->first = itemList;
		this->last = itemList;
		return;
	}

	ArraListItem * itemListAux = this->first;

	bool change = false;
	// Para acelerar inserciones, miramos si lo podemos insertar en ultima posicion
	// Este algoritmo es super optimizable, pero para salir del paso lo dejo asi
	if (!(this->last->key->ToString() > key->ToString())) {
		itemListAux = this->last;
	} else {
		while (itemListAux->next != NULL) {
			if (itemListAux->key->ToString() > key->ToString()) {
				change = true;
				break;
			}

			itemListAux = itemListAux->next;
		}
	}

	if (change) {
		itemList->key = itemListAux->key;
		itemList->value = itemListAux->value;
		itemListAux->key = key;
		itemListAux->value = value;
	} else {
		this->last = itemList;
	}

	itemList->next = itemListAux->next;
	itemListAux->next = itemList;
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

	ArraListItem * itemListAux = this->first;

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

	ArraListItem * itemListAux = this->first;

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
	ArraListItem * itemListAux = this->first;
	ArraListItem * last = NULL;

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
