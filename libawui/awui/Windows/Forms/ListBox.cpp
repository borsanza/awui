// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "ListBox.h"

using namespace awui::Windows::Forms;

ListBox::ListBox() {
}

ListBox::~ListBox() {
}

int ListBox::IsClass(Classes::Enum objectClass) const {
	if (objectClass == Classes::ListBox)
		return 1;

	return Control::IsClass(objectClass);
}
