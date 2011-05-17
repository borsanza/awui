// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiControl.h"

class awuiSplitter;
class awuiPanel;

class awuiSplitContainer : public awuiControl {
private:
  awuiSplitter * splitter;
  awuiPanel * panel1;
  awuiPanel * panel2;

public:
  awuiSplitContainer();
  ~awuiSplitContainer();
};