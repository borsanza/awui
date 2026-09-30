/**
 * awui/Windows/Forms/Station/Browser/Page.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "Page.h"

#include <algorithm>
#include <vector>

using namespace awui::Windows::Forms::Station;
using namespace awui::Windows::Forms::Input;

Page::Page() {
}

bool Page::OnKeyPress(Keys::Enum key) {
	if ((key != Keys::Key_HOME) && (key != Keys::Key_END) && (key != Keys::Key_PAGEUP) && (key != Keys::Key_PAGEDOWN)) {
		return false;
	}

	// Filas a las que se puede ir, en orden (se añaden de arriba abajo)
	std::vector<Control *> rows;
	int current = -1;
	for (int i = 0; i < GetCount(); i++) {
		Control *child = Get(i);
		if (!child->GetVisible() || !child->IsFocusable()) {
			continue;
		}

		if (child == GetFocused()) {
			current = rows.size();
		}

		rows.push_back(child);
	}

	if (rows.empty()) {
		return false;
	}

	// Filas que caben en lo que se ve de la página (el Browser que la contiene)
	int rowHeight = rows[0]->GetHeight();
	int pageRows = 5;
	if (GetParent() && (rowHeight > 0)) {
		pageRows = std::max(1, (GetParent()->GetHeight() / rowHeight) - 1);
	}

	int last = rows.size() - 1;
	int target = 0;
	switch (key) {
		case Keys::Key_HOME:
			target = 0;
			break;
		case Keys::Key_END:
			target = last;
			break;
		case Keys::Key_PAGEUP:
			target = std::max(0, ((current < 0) ? 0 : current) - pageRows);
			break;
		case Keys::Key_PAGEDOWN:
			target = std::min(last, ((current < 0) ? 0 : current) + pageRows);
			break;
		default:
			break;
	}

	if (target != current) {
		rows[target]->SetFocus();
	}

	return true;
}
