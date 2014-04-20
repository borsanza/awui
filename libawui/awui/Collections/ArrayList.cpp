// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "ArrayList.h"

#include <awui/String.h>
#include <stdlib.h>

using namespace awui::Collections;

ArrayList::ArrayList() {
	this->first = NULL;
	this->count = 0;
}

ArrayList::~ArrayList() {
	this->Clear();
}

int ArrayList::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::ArrayList)
		return 1;

	return Object::IsClass(objectClass);
}

awui::String ArrayList::ToString() {
	return "awui.Collections.ArrayList";
}

void ArrayList::Add(Object * item) {
	ArrayListItem * itemList = (ArrayListItem *) malloc(sizeof (struct ArrayListItem));
	itemList->object = item;
	itemList->next = NULL;
	this->count++;

	if (this->first == NULL) {
		this->first = itemList;
		return;
	}

	ArrayListItem * itemListAux = this->first;

	while (itemListAux->next != NULL)
		itemListAux = itemListAux->next;

	itemListAux->next = itemList;
}

int ArrayList::GetCount() {
	return this->count;
}

void ArrayList::Clear() {
	while (this->GetCount() > 0)
		this->RemoveAt(0);
}

int ArrayList::IndexOf(Object * item) {
	int pos = 0;

	ArrayListItem * itemListAux = this->first;

	while (itemListAux != NULL) {
		if (itemListAux->object == item)
			return pos;

		pos++;
		itemListAux = itemListAux->next;
	}

	return -1;
}

awui::Object * ArrayList::Get(int index) {
	int pos = 0;

	ArrayListItem * itemListAux = this->first;

	while (itemListAux != NULL) {
		if (pos == index)
			return itemListAux->object;

		pos++;
		itemListAux = itemListAux->next;
	}

	return NULL;
}

void ArrayList::Remove(Object * item) {
	ArrayListItem * itemListAux = this->first;
	ArrayListItem * last = NULL;

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

void ArrayList::RemoveAt(int index) {
	int pos = 0;
	ArrayListItem * itemListAux = this->first;
	ArrayListItem * last = NULL;

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

void ArrayList::SetChildIndex(Object * item, int index) {
	ArrayListItem * itemListAux = this->first;
	ArrayListItem * last = NULL;

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

	ArrayListItem * itemListAux2 = this->first;

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
