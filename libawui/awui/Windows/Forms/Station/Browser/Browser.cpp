/**
 * awui/Windows/Forms/Station/Browser/Browser.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "Browser.h"

#include <awui/Windows/Forms/ControlCollection.h>
#include <awui/Windows/Forms/Form.h>
#include <awui/Windows/Forms/Station/Browser/Page.h>

using namespace awui::Drawing;
using namespace awui::Windows::Forms::Station::Browser;

#define GRADIENT_WIDTH 64

Browser::Browser() {
	// this->SetBackColor(Color::FromArgb(255, 255, 255));
	this->_page = NULL;

	this->_gradientUp.SetColor(0, ColorF::FromArgb(0, 0, 0, 0));
	this->_gradientUp.SetColor(1, ColorF::FromArgb(0, 0, 0, 0));
	this->_gradientUp.SetColor(2, ColorF::FromArgb(0, 0, 0, 0));
	this->_gradientUp.SetColor(3, ColorF::FromArgb(0, 0, 0, 0));
	this->_gradientUp.SetDock(DockStyle::None);

	this->_gradientBottom.SetColor(0, ColorF::FromArgb(0, 0, 0, 0));
	this->_gradientBottom.SetColor(1, ColorF::FromArgb(0, 0, 0, 0));
	this->_gradientBottom.SetColor(2, ColorF::FromArgb(0, 0, 0, 0));
	this->_gradientBottom.SetColor(3, ColorF::FromArgb(0, 0, 0, 0));
	this->_gradientBottom.SetDock(DockStyle::None);

	this->_gradientLeft.SetColor(0, ColorF::FromArgb(0, 0, 0, 0));
	this->_gradientLeft.SetColor(1, ColorF::FromArgb(0, 0, 0, 0));
	this->_gradientLeft.SetColor(2, ColorF::FromArgb(0, 0, 0, 0));
	this->_gradientLeft.SetColor(3, ColorF::FromArgb(0, 0, 0, 0));
	this->_gradientLeft.SetDock(DockStyle::None);

	this->_gradientRight.SetColor(0, ColorF::FromArgb(0, 0, 0, 0));
	this->_gradientRight.SetColor(1, ColorF::FromArgb(0, 0, 0, 0));
	this->_gradientRight.SetColor(2, ColorF::FromArgb(0, 0, 0, 0));
	this->_gradientRight.SetColor(3, ColorF::FromArgb(0, 0, 0, 0));
	this->_gradientRight.SetDock(DockStyle::None);

	this->GetControls()->Add(&this->_gradientUp);
	this->GetControls()->Add(&this->_gradientBottom);
	this->GetControls()->Add(&this->_gradientLeft);
	this->GetControls()->Add(&this->_gradientRight);
}

Browser::~Browser() {
}

void Browser::OnTick() {
	Control * selected = Form::GetControlSelected();

	if (selected->GetParent() != this->_page)
		return;

	int xCenterW = (selected->GetLeft() + selected->GetRight()) >> 1;
	int yCenterW = (selected->GetTop() + selected->GetBottom()) >> 1;
	int xCenter = this->GetWidth() >> 1;
	int yCenter = this->GetHeight() >> 1;

	int left = xCenter - xCenterW;
	int top = yCenter - yCenterW;
	if ((left + this->_page->GetWidth()) <= this->GetWidth())
		left = this->GetWidth() - this->_page->GetWidth();
	if ((top + this->_page->GetHeight()) <= this->GetHeight())
		top = this->GetHeight() - this->_page->GetHeight();
	if (left > 0) left = 0;
	if (top > 0) top = 0;

	this->_page->SetLocationGo(left, top);

	this->_gradientUp.SetLocation(0, 0);
	this->_gradientUp.SetWidth(this->GetWidth());
	this->_gradientLeft.SetLocation(0, 0);
	this->_gradientLeft.SetHeight(this->GetHeight());
	this->_gradientRight.SetLocation(this->GetWidth() - GRADIENT_WIDTH, 0);
	this->_gradientRight.SetHeight(this->GetHeight());
	this->_gradientBottom.SetLocation(0, this->GetHeight() - GRADIENT_WIDTH);
	this->_gradientBottom.SetWidth(this->GetWidth());

	bool alpha;

	alpha = this->_page->GetTopGo() < 0;
	this->_gradientUp.SetColorGo(0, ColorF::FromArgb(alpha ? 255 : 0, 0,   0, 0));
	this->_gradientUp.SetColorGo(1, ColorF::FromArgb(alpha ? 255 : 0, 0,   0, 0));
	this->_gradientUp.SetHeight(alpha ? GRADIENT_WIDTH : 0);

	alpha = this->_page->GetBottomGo() > this->GetHeight();
	this->_gradientBottom.SetColorGo(2, ColorF::FromArgb(alpha ? 255 : 0, 0,   0, 0));
	this->_gradientBottom.SetColorGo(3, ColorF::FromArgb(alpha ? 255 : 0, 0,   0, 0));
	this->_gradientBottom.SetHeight(alpha ? GRADIENT_WIDTH : 0);

	alpha = this->_page->GetLeftGo() < 0;
	this->_gradientLeft.SetColorGo(0, ColorF::FromArgb(alpha ? 255 : 0, 0,   0, 0));
	this->_gradientLeft.SetColorGo(3, ColorF::FromArgb(alpha ? 255 : 0, 0,   0, 0));
	this->_gradientLeft.SetWidth(alpha ? GRADIENT_WIDTH : 0);

	alpha = this->_page->GetRightGo() > this->GetWidth();
	this->_gradientRight.SetColorGo(1, ColorF::FromArgb(alpha ? 255 : 0, 0,   0, 0));
	this->_gradientRight.SetColorGo(2, ColorF::FromArgb(alpha ? 255 : 0, 0,   0, 0));
	this->_gradientRight.SetWidth(alpha ? GRADIENT_WIDTH : 0);
}

void Browser::SetPage(Page * page) {
	if (this->_page == page)
		return;

	if (this->_page)
		this->GetControls()->Remove(this->_page);

	this->_page = page;
	this->GetControls()->Add(this->_page);

	this->GetControls()->MoveToEnd(&this->_gradientUp);
	this->GetControls()->MoveToEnd(&this->_gradientBottom);
	this->GetControls()->MoveToEnd(&this->_gradientLeft);
	this->GetControls()->MoveToEnd(&this->_gradientRight);
}
