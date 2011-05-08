// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiArrayList.h"
#include "awuiBitmap.h"
#include "awuiColor.h"
#include "awuiControl.h"
#include <stdlib.h>

awuiControl::awuiControl() {
	this->controls = new awuiArrayList();
	this->x = 0;
	this->y = 0;
	this->width = 100;
	this->height = 100;
	this->bitmap = NULL;
	this->backColor = awuiColor::FromArgb(226, 226, 226);
	this->OnResizePre();
}

awuiControl::~awuiControl() {
	for (int i = 0; i < this->controls->GetCount(); i++)
		delete this->controls->Get(i);

	this->controls->Clear();
	delete this->controls;

	if (this->bitmap != NULL)
		delete this->bitmap;

	if (this->backColor != NULL)
		delete this->backColor;
}

void awuiControl::SetTop(int y) {
	this->SetLocation(this->x, y);
}

void awuiControl::SetLeft(int x) {
	this->SetLocation(x, this->y);
}

void awuiControl::SetLocation(int x, int y) {
	this->SetBounds(x, y, this->width, this->height);
}

void awuiControl::SetWidth(int width) {
	this->SetSize(width, this->height);
}

void awuiControl::SetHeight(int height) {
	this->SetSize(this->width, height);
}

void awuiControl::SetSize(int width, int height) {
	this->SetBounds(this->x, this->y, width, height);
}

void awuiControl::SetBounds(int x, int y, int width, int height) {
	if ((this->x == x) && (this->y == y) && (this->width == width) && (this->height == height))
		return;

	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;

	this->OnResizePre();
}

int awuiControl::GetTop() {
	return this->y;
}

int awuiControl::GetLeft() {
	return this->x;
}

void awuiControl::GetLocation(int &x, int &y) {
	x = this->GetLeft();
	y = this->GetTop();
}

int awuiControl::GetWidth() {
	return this->width;
}

int awuiControl::GetHeight() {
	return this->height;
}

void awuiControl::GetSize(int &width, int &height) {
	width = this->GetWidth();
	height = this->GetHeight();
}

void awuiControl::GetBounds(int &x, int &y, int &width, int &height) {
	this->GetLocation(x, y);
	this->GetSize(width, height);
}

awuiArrayList * awuiControl::GetControls() {
	return this->controls;
}

void awuiControl::OnResizePre() {
	if (this->bitmap != NULL)
		delete this->bitmap;

	this->bitmap = new awuiBitmap(this->GetWidth(), this->GetHeight());
}

void awuiControl::SetBackColor(awuiColor * color) {
	if (this->backColor != NULL)
		delete this->backColor;

	this->backColor = awuiColor::FromArgb(color->ToArgb());
}

awuiColor * awuiControl::GetBackColor() {
	return awuiColor::FromArgb(this->backColor->ToArgb());
}