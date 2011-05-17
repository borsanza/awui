// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiSplitter.h"
#include "awuiColor.h"

awuiSplitter::awuiSplitter() {
  awuiColor * color = awuiColor::FromArgb(0, 255, 0);
  this->SetBackColor(color);
  delete color;
}

awuiSplitter::~awuiSplitter() {
}