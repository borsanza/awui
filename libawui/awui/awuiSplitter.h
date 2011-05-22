// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiControl.h"
#include "awuiSplitContainer.h"

class awuiMouseEventArgs;

class awuiSplitter : public awuiControl {
private:
		awuiSplitContainer::Orientation orientation;

public:
  awuiSplitter();
  ~awuiSplitter();
	
	virtual int IsClass(awuiClasses objectClass);
	
	awuiSplitContainer::Orientation GetOrientation();
	void SetOrientation(awuiSplitContainer::Orientation orientation);
	
	virtual void OnMouseMove(awuiMouseEventArgs * e);
};