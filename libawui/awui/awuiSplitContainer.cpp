// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiSplitContainer.h"

#include "awuiArrayList.h"
#include "awuiColor.h"
#include "awuiPanel.h"
#include "awuiSplitter.h"

awuiSplitContainer::awuiSplitContainer() {
  awuiColor * color = awuiColor::FromArgb(255, 255, 0);
  this->SetBackColor(color);
  delete color;
	
	this->SetSize(200, 200);
	
  this->panel1 = new awuiPanel();
  this->splitter = new awuiSplitter();
  this->panel2 = new awuiPanel();
  this->GetControls()->Add(this->panel1);
  this->GetControls()->Add(this->splitter);
  this->GetControls()->Add(this->panel2);
}

awuiSplitContainer::~awuiSplitContainer() {
  delete this->panel1;
  delete this->splitter;
  delete this->panel2;
}