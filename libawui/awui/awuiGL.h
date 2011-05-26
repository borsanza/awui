// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiRectangle.h"

class awuiGL {
	friend class awuiForm;
	friend class awuiControl;

private:
	awuiRectangle rectangle1;
	awuiRectangle rectangle2;

	awuiGL();
	void SetClippingBase(awuiRectangle rectangle);
	awuiRectangle GetClippingBase();

public:
	void SetClipping(awuiRectangle rectangle);
	awuiRectangle GetClipping();

	awuiRectangle GetClippingResult();

	void SetClipping();
};