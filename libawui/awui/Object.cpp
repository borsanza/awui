/*
 * awui/Object.cpp
 *
 * Copyright (C) 2011 Borja Sánchez Zamorano
 */

#include "Object.h"

#include <awui/String.h>

using namespace awui;

Object::Object() {
  
}

Object::~Object() {
}

bool Object::IsClass(Classes objectClass) const {
	if (objectClass == Classes::Object) {
		return true;
	}

	return false;
}

String Object::ToString() {
	return "awui.Object";
}
