/*
 * awui/Windows/Forms/SplitContainer.cpp
 *
 * Copyright (C) 2013 Borja Sánchez Zamorano
 */

#include "SplitContainer.h"

#include <awui/Core/Color.h>
#include <awui/Windows/Forms/Panel.h>
#include <awui/Windows/Forms/Splitter.h>

#include <iostream>

using namespace awui::Drawing;
using namespace awui::Windows::Forms;

SplitContainer::SplitContainer() {
	m_class = Classes::SplitContainer;
	m_splitterDistance = 100;
	m_splitterIncrement = 1;
	m_splitterWidth = 4;
	m_orientation = Orientation::Vertical;

	SetName("SplitContainer");

	SetBackColor(Color::FromArgb(0, 0, 0, 0));

	m_panel1 = new Panel();
	m_splitter = new Splitter();
	m_splitter->SetOrientation(m_orientation);
	m_panel2 = new Panel();
	AddWidget(m_panel1);
	AddWidget(m_splitter);
	AddWidget(m_panel2);

	SetSize(200, 200);
}

bool SplitContainer::IsClass(Classes objectClass) const {
	return (objectClass == Classes::SplitContainer) || Control::IsClass(objectClass);
}

Panel *SplitContainer::GetPanel1() const {
	return m_panel1;
}

Panel *SplitContainer::GetPanel2() const {
	return m_panel2;
}

void SplitContainer::SetLocationW(Control *control, int pos) {
	if (m_orientation == Orientation::Vertical)
		control->SetLocation(pos, 0);
	else
		control->SetLocation(0, pos);
}

void SplitContainer::SetSizeW(Control *control, int size, int substract) {
	if (m_orientation == Orientation::Vertical) {
		if (substract)
			size = GetWidth() - size;

		control->SetSize(size, GetHeight());
	} else {
		if (substract)
			size = GetHeight() - size;

		control->SetSize(GetWidth(), size);
	}
}

int SplitContainer::GetSizeW(Control *control) const {
	if (m_orientation == Orientation::Vertical)
		return control->GetWidth();
	else
		return control->GetHeight();
}

int SplitContainer::GetMinimumSizeW(Control *control) const {
	if (m_orientation == Orientation::Vertical)
		return control->GetMinimumSize().GetWidth();
	else
		return control->GetMinimumSize().GetHeight();
}

void SplitContainer::RecalculatePositions() {
	int distance = m_splitterDistance;
	int x2 = m_splitterWidth >> 1;
	int x1 = m_splitterWidth - x2;

	int size = (m_orientation == Orientation::Vertical) ? GetWidth() : GetHeight();

	int minSize = GetMinimumSizeW(m_panel1);
	if (distance < (minSize + x1))
		distance = (minSize + x1);

	minSize = GetMinimumSizeW(m_panel2);
	if (distance > (size - minSize - x1))
		distance = (size - minSize - x1);

	SetLocationW(m_panel1, 0);
	SetSizeW(m_panel1, distance - x1, 0);

	SetLocationW(m_splitter, distance - x1);
	SetSizeW(m_splitter, m_splitterWidth, 0);

	SetLocationW(m_panel2, distance + x2);
	SetSizeW(m_panel2, (distance + x2), 1);
}

float SplitContainer::GetSplitterDistance() const {
	return m_splitterDistance;
}

void SplitContainer::SetSplitterDistance(int distance) {
	m_splitterDistance = distance;
	RecalculatePositions();
}

int SplitContainer::GetSplitterIncrement() const {
	return m_splitterIncrement;
}

void SplitContainer::SetSplitterIncrement(int increment) {
	m_splitterIncrement = increment;
}

int SplitContainer::GetSplitterWidth() const {
	return m_splitterWidth;
}

void SplitContainer::SetSplitterWidth(int width) {
	m_splitterWidth = width;
}

SplitContainer::Orientation SplitContainer::GetOrientation() const {
	return m_orientation;
}

void SplitContainer::SetOrientation(Orientation orientation) {
	m_orientation = orientation;
	m_splitter->SetOrientation(orientation);
	RecalculatePositions();
}

void SplitContainer::Layout() {
	RecalculatePositions();
}
