// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiObject.h"

class awuiImage : public awuiObject {
	friend class awuiGraphics;
	friend class awuiForm;

protected:
	unsigned char *image;
	int width;
	int height;
	int btpp;

public:
	awuiImage();
	~awuiImage();

	virtual int IsClass(awuiClasses objectClass);

	int GetWidth();
	int GetHeight();
};