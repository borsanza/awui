// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiControl.h"

class awuiButton : public awuiControl {
public:
	awuiButton();
	~awuiButton();

	virtual int IsClass(awuiClasses objectClass);

	float GetAnimationValue();
	virtual void OnPaint(awuiGL * gl);
};