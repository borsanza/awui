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

int Object::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::Object)
		return 1;

	return 0;
}

String Object::ToString() {
	return "awui.Object";
}
