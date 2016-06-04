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

Browser::Browser() {
//	this->SetBackColor(Color::FromArgb(255, 255, 255));
	this->_page = NULL;

	this->_gradientUp.SetColor1(Color::FromArgb(255, 0,   0, 0));
	this->_gradientUp.SetColor2(Color::FromArgb(255, 0,   0, 0));
	this->_gradientUp.SetColor3(Color::FromArgb(0,  0, 0, 0));
	this->_gradientUp.SetColor4(Color::FromArgb(0,  0, 0, 0));
	this->_gradientUp.SetDock(DockStyle::None);

	this->_gradientBottom.SetColor1(Color::FromArgb(0,  0, 0, 0));
	this->_gradientBottom.SetColor2(Color::FromArgb(0,  0, 0, 0));
	this->_gradientBottom.SetColor3(Color::FromArgb(255, 0,   0, 0));
	this->_gradientBottom.SetColor4(Color::FromArgb(255, 0,   0, 0));
	this->_gradientBottom.SetDock(DockStyle::None);

	this->_gradientLeft.SetColor1(Color::FromArgb(255,  0, 0, 0));
	this->_gradientLeft.SetColor2(Color::FromArgb(0,  0, 0, 0));
	this->_gradientLeft.SetColor3(Color::FromArgb(0, 0,   0, 0));
	this->_gradientLeft.SetColor4(Color::FromArgb(255, 0,   0, 0));
	this->_gradientLeft.SetDock(DockStyle::None);

	this->_gradientRight.SetColor1(Color::FromArgb(0,  0, 0, 0));
	this->_gradientRight.SetColor2(Color::FromArgb(255,  0, 0, 0));
	this->_gradientRight.SetColor3(Color::FromArgb(255, 0,   0, 0));
	this->_gradientRight.SetColor4(Color::FromArgb(0, 0,   0, 0));
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
	int xCenterW = (selected->GetLeft() + selected->GetRight()) >> 1;
	int yCenterW = (selected->GetTop() + selected->GetBottom()) >> 1;
	int xCenterPage = this->_page->GetWidth() >> 1;
	int yCenterPage = this->_page->GetHeight() >> 1;
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

	this->_gradientUp.SetSize(this->GetWidth(), 50);
	this->_gradientBottom.SetSize(this->GetWidth(), 50);
	this->_gradientLeft.SetSize(50, this->GetHeight());
	this->_gradientRight.SetSize(50, this->GetHeight());

	if (this->_page->GetTop() < 0)
		this->_gradientUp.SetLocation(0, 0);
	else
		this->_gradientUp.SetLocation(0, -this->_gradientUp.GetHeight());

	if (this->_page->GetBottom() > this->GetBottom())
		this->_gradientBottom.SetLocation(0, this->GetHeight() - this->_gradientBottom.GetHeight());
	else
		this->_gradientBottom.SetLocation(0, this->GetHeight());

	if (this->_page->GetLeft() < 0)
		this->_gradientLeft.SetLocation(0, 0);
	else
		this->_gradientLeft.SetLocation(- this->_gradientLeft.GetWidth(), 0);

	if (this->_page->GetRight() > this->GetRight())
		this->_gradientRight.SetLocation(this->GetWidth() - this->_gradientRight.GetWidth(), 0);
	else
		this->_gradientRight.SetLocation(this->GetWidth(), 0);

}

void Browser::SetPage(Page * page) {
	if (this->_page) {
		this->GetControls()->Remove(this->_page);
		this->GetControls()->Remove(&this->_gradientUp);
		this->GetControls()->Remove(&this->_gradientBottom);
		this->GetControls()->Remove(&this->_gradientLeft);
		this->GetControls()->Remove(&this->_gradientRight);
	}

	this->_page = page;
	this->GetControls()->Add(this->_page);
	this->GetControls()->Add(&this->_gradientUp);
	this->GetControls()->Add(&this->_gradientBottom);
	this->GetControls()->Add(&this->_gradientLeft);
	this->GetControls()->Add(&this->_gradientRight);
}
