/**
 * awui/Windows/Forms/Station/Browser/Browser.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "Browser.h"

#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/Station/Browser/Page.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms::Station::Browser;

#define GRADIENT_WIDTH 64

Browser::Browser() {
	m_class = Classes::Browser;
	// SetBackColor(Color::FromArgb(0, 0, 0));
	m_page = NULL;

	m_gradientUp.SetColor(0, ColorF::FromArgb(0, 0, 0, 0));
	m_gradientUp.SetColor(1, ColorF::FromArgb(0, 0, 0, 0));
	m_gradientUp.SetColor(2, ColorF::FromArgb(0, 0, 0, 0));
	m_gradientUp.SetColor(3, ColorF::FromArgb(0, 0, 0, 0));
	m_gradientUp.SetDock(DockStyle::None);

	m_gradientBottom.SetColor(0, ColorF::FromArgb(0, 0, 0, 0));
	m_gradientBottom.SetColor(1, ColorF::FromArgb(0, 0, 0, 0));
	m_gradientBottom.SetColor(2, ColorF::FromArgb(0, 0, 0, 0));
	m_gradientBottom.SetColor(3, ColorF::FromArgb(0, 0, 0, 0));
	m_gradientBottom.SetDock(DockStyle::None);

	m_gradientLeft.SetColor(0, ColorF::FromArgb(0, 0, 0, 0));
	m_gradientLeft.SetColor(1, ColorF::FromArgb(0, 0, 0, 0));
	m_gradientLeft.SetColor(2, ColorF::FromArgb(0, 0, 0, 0));
	m_gradientLeft.SetColor(3, ColorF::FromArgb(0, 0, 0, 0));
	m_gradientLeft.SetDock(DockStyle::None);

	m_gradientRight.SetColor(0, ColorF::FromArgb(0, 0, 0, 0));
	m_gradientRight.SetColor(1, ColorF::FromArgb(0, 0, 0, 0));
	m_gradientRight.SetColor(2, ColorF::FromArgb(0, 0, 0, 0));
	m_gradientRight.SetColor(3, ColorF::FromArgb(0, 0, 0, 0));
	m_gradientRight.SetDock(DockStyle::None);

	AddWidget(&m_gradientUp);
	AddWidget(&m_gradientBottom);
	AddWidget(&m_gradientLeft);
	AddWidget(&m_gradientRight);
}

bool Browser::IsClass(Classes objectClass) const {
	return (objectClass == Classes::Browser) || Control::IsClass(objectClass);
}

void Browser::OnTick(float deltaSeconds) {
	Control *selected = nullptr;

	Form *form = GetForm();
	if (form) {
		selected = form->GetChildFocused();
	}

	if (!selected || (selected->GetParent() != m_page)) {
		return;
	}

	int xCenterW = (selected->GetLeft() + selected->GetRight()) >> 1;
	int yCenterW = (selected->GetTop() + selected->GetBottom()) >> 1;
	int xCenter = GetWidth() >> 1;
	int yCenter = GetHeight() >> 1;

	int left = std::min(std::max(xCenter - xCenterW, GetWidth() - m_page->GetWidth()), 0);
	int top = std::min(std::max(yCenter - yCenterW, GetHeight() - m_page->GetHeight()), 0);

	m_page->SetLocationGo(left, top);

	m_gradientUp.SetLocation(0, 0);
	m_gradientUp.SetWidth(GetWidth());
	m_gradientLeft.SetLocation(0, 0);
	m_gradientLeft.SetHeight(GetHeight());
	m_gradientRight.SetLocation(GetWidth() - GRADIENT_WIDTH, 0);
	m_gradientRight.SetHeight(GetHeight());
	m_gradientBottom.SetLocation(0, GetHeight() - GRADIENT_WIDTH);
	m_gradientBottom.SetWidth(GetWidth());

	bool alpha;

	alpha = m_page->GetTopGo() < 0;
	m_gradientUp.SetColorGo(0, ColorF::FromArgb(alpha ? 255 : 0, 0, 0, 0));
	m_gradientUp.SetColorGo(1, ColorF::FromArgb(alpha ? 255 : 0, 0, 0, 0));
	m_gradientUp.SetHeight(alpha ? GRADIENT_WIDTH : 0);

	alpha = m_page->GetBottomGo() > GetHeight();
	m_gradientBottom.SetColorGo(2, ColorF::FromArgb(alpha ? 255 : 0, 0, 0, 0));
	m_gradientBottom.SetColorGo(3, ColorF::FromArgb(alpha ? 255 : 0, 0, 0, 0));
	m_gradientBottom.SetHeight(alpha ? GRADIENT_WIDTH : 0);

	alpha = m_page->GetLeftGo() < 0;
	m_gradientLeft.SetColorGo(0, ColorF::FromArgb(alpha ? 255 : 0, 0, 0, 0));
	m_gradientLeft.SetColorGo(3, ColorF::FromArgb(alpha ? 255 : 0, 0, 0, 0));
	m_gradientLeft.SetWidth(alpha ? GRADIENT_WIDTH : 0);

	alpha = m_page->GetRightGo() > GetWidth();
	m_gradientRight.SetColorGo(1, ColorF::FromArgb(alpha ? 255 : 0, 0, 0, 0));
	m_gradientRight.SetColorGo(2, ColorF::FromArgb(alpha ? 255 : 0, 0, 0, 0));
	m_gradientRight.SetWidth(alpha ? GRADIENT_WIDTH : 0);
}

void Browser::SetPage(Page *page) {
	if (m_page == page)
		return;

	if (m_page)
		RemoveWidget(m_page);

	m_page = page;
	AddWidget(m_page);

	MoveToEnd(&m_gradientUp);
	MoveToEnd(&m_gradientBottom);
	MoveToEnd(&m_gradientLeft);
	MoveToEnd(&m_gradientRight);
}
