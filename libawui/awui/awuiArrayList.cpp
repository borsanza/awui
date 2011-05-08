// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiArrayList.h"
#include <stdlib.h>

awuiArrayList::awuiArrayList() {
	this->first = NULL;
	this->count = 0;
}

awuiArrayList::~awuiArrayList() {
	this->Clear();
}

void awuiArrayList::Add(void * item) {
	awuiArraListItem * itemList = (awuiArraListItem *) malloc(sizeof (awuiArraListItem *));
	itemList->object = item;
	itemList->next = NULL;
	this->count++;

	if (this->first == NULL) {
		this->first = itemList;
		return;
	}

	awuiArraListItem * itemListAux = this->first;

	while (itemListAux->next != NULL)
		itemListAux = itemListAux->next;

	itemListAux->next = itemList;
}

int awuiArrayList::GetCount() {
	return this->count;
}

void awuiArrayList::Clear() {
	while (this->GetCount() > 0)
		this->RemoveAt(0);
}

int awuiArrayList::IndexOf(void * item) {
	int pos = 0;

	awuiArraListItem * itemListAux = this->first;

	while (itemListAux != NULL) {
		if (itemListAux->object == item)
			return pos;

		pos++;
		itemListAux = itemListAux->next;
	}

	return -1;
}

void * awuiArrayList::Get(int index) {
	int pos = 0;

	awuiArraListItem * itemListAux = this->first;

	while (itemListAux != NULL) {
		if (pos == index)
			return itemListAux->object;

		pos++;
		itemListAux = itemListAux->next;
	}

	return NULL;
}

void awuiArrayList::Remove(void * item) {
	awuiArraListItem * itemListAux = this->first;
	awuiArraListItem * last = NULL;

	while (itemListAux != NULL) {
		if (itemListAux->object == item) {
			this->count--;

			if (last == NULL)
				this->first = itemListAux->next;
			else
				last->next = itemListAux->next;

			free(itemListAux);
			return;
		}

		last = itemListAux;
		itemListAux = itemListAux->next;
	}
}

void awuiArrayList::RemoveAt(int index) {
	int pos = 0;
	awuiArraListItem * itemListAux = this->first;
	awuiArraListItem * last = NULL;

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

void awuiArrayList::SetChildIndex(void * item, int index) {
	awuiArraListItem * itemListAux = this->first;
	awuiArraListItem * last = NULL;

	while (itemListAux != NULL) {
		if (itemListAux->object == item) {
			if (last == NULL)
				this->first = itemListAux->next;
			else
				last->next = itemListAux->next;

			break;
		}

		last = itemListAux;
		itemListAux = itemListAux->next;
	}

	// Sino encontramos el item, descartamos su inserción
	if (itemListAux == NULL)
		return;

	int pos = 0;

	awuiArraListItem * itemListAux2 = this->first;

	while (itemListAux2 != NULL) {
		if (pos == index) {
			itemListAux->next = itemListAux2;
			if (last == NULL)
				this->first = itemListAux;
			else
				last->next = itemListAux;

			break;
		}

		last = itemListAux2;
		itemListAux2 = itemListAux2->next;
	}

	// Si estamos en esta situación es que no se ha podido insertar,
	// lo insertamos el ultimo
	if (itemListAux2 == NULL) {
		last->next = itemListAux;
		itemListAux->next = NULL;
	}
}