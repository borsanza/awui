// (c) Copyright 2024 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "EventArgs.h"

using namespace awui::Windows::Forms;

EventArgs::EventArgs() {
	m_class = Classes::EventArgs;
}

bool EventArgs::IsClass(Classes objectClass) const {
	return (objectClass == Classes::EventArgs) || Object::IsClass(objectClass);
}
