// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiControl.h"
#include "awuiSplitContainer.h"

class awuiMouseEventArgs;

class awuiSplitter : public awuiControl {
private:
		awuiSplitContainer::Orientation orientation;
		int mouseActive;

public:
  awuiSplitter();
  ~awuiSplitter();
	
	virtual int IsClass(awuiClasses objectClass) const;
	
	awuiSplitContainer::Orientation GetOrientation();
	void SetOrientation(awuiSplitContainer::Orientation orientation);
	
	virtual void OnMouseDown(awuiMouseEventArgs * e);
	virtual void OnMouseMove(awuiMouseEventArgs * e);
	virtual void OnMouseUp(awuiMouseEventArgs * e);
	virtual void OnMouseEnter();
	virtual void OnMouseLeave();
};