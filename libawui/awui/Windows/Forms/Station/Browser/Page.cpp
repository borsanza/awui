/**
 * awui/Windows/Forms/Station/Browser/Page.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "Page.h"

using namespace awui::Windows::Forms::Station::Browser;

Page::Page() {
	m_class = Classes::Page;
}

bool Page::IsClass(Classes objectClass) const {
	return (objectClass == Classes::Page) || Control::IsClass(objectClass);
}
