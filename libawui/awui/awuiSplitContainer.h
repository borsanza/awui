// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUISPLITCONTAINER_H__
#define __AWUISPLITCONTAINER_H__

#include "awuiControl.h"

class awuiSplitter;
class awuiPanel;

class awuiSplitContainer : public awuiControl {
public:
	enum Orientation {
		Vertical,
		Horizontal
	};

private:
  awuiSplitter * splitter;
  awuiPanel * panel1;
  awuiPanel * panel2;
	
	float splitterDistance;
	int splitterIncrement;
	int splitterWidth;
	Orientation orientation;

	void RecalculatePositions();

public:
  awuiSplitContainer();
  ~awuiSplitContainer();

	virtual int IsClass(awuiClasses objectClass);
	
  awuiPanel * GetPanel1();
  awuiPanel * GetPanel2();

	float GetSplitterDistance();
	void SetSplitterDistance(int distance);

	int GetSplitterIncrement();
	void SetSplitterIncrement(int increment);

	int GetSplitterWidth();
	void SetSplitterWidth(int width);

	Orientation GetOrientation();
	void SetOrientation(Orientation orientation);
	
	virtual void Layout();
};

#endif