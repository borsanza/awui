/*
 * awui/Object.cpp
 *
 * Copyright (C) 2011 Borja Sánchez Zamorano
 */

#include "Object.h"

#include <awui/String.h>

using namespace awui;

bool Object::IsClass(Classes objectClass) const {
	return objectClass == Classes::Object;
}

String Object::ToString() const {
	return "awui.Object";
}
