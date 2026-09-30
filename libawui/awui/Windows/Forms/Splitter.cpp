// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "Splitter.h"

#include <awui/Drawing/Color.h>
#include <awui/Windows/Forms/MouseEventArgs.h>
#include <awui/Windows/Forms/SplitContainer.h>

#include <iostream>

using namespace awui::Drawing;
using namespace awui::Windows::Forms;

Splitter::Splitter() {
	m_orientation = SplitContainer::Orientation::Horizontal;
	SetBackColor(Color::FromArgb(255, 255, 255));
	SetName("Splitter");
	SetSize(20, 200);
	m_mouseActive = false;
}

SplitContainer::Orientation Splitter::GetOrientation() const {
	return m_orientation;
}

void Splitter::SetOrientation(SplitContainer::Orientation orientation) {
	m_orientation = orientation;
}

void Splitter::OnMouseDown(MouseEventArgs *e) {
	if (e->GetButton() == MouseButtons::Left)
		m_mouseActive = true;
}

void Splitter::OnMouseMove(MouseEventArgs *e) {
	if (!m_mouseActive)
		return;

	if (dynamic_cast<SplitContainer *>(GetParent())) {
		if (m_orientation == SplitContainer::Orientation::Vertical)
			((SplitContainer *) GetParent())->SetSplitterDistance(GetLeft() + e->GetX());
		else
			((SplitContainer *) GetParent())->SetSplitterDistance(GetTop() + e->GetY());
	}

	//	std::cout << "Motion: " << e->GetX() << "x" << e->GetY() << "   " << GetName() << std::endl;
}

void Splitter::OnMouseUp(MouseEventArgs *e) {
	m_mouseActive = false;
}

void Splitter::OnMouseEnter() {
	//	std::cout << "OnMouseEnter" << std::endl;
}

void Splitter::OnMouseLeave() {
	//	std::cout << "OnMouseLeave" << std::endl;
}
