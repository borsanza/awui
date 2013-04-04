// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiObject.h"

typedef struct _cairo_surface cairo_surface_t;
typedef struct _cairo cairo_t;

class awuiImage : public awuiObject {
	friend class awuiGraphics;
	friend class awuiForm;
	friend class awuiButton;
	friend class awuiGL;

protected:
	unsigned char *image;
	int width;
	int height;
	int btpp;
	cairo_surface_t *cairo_surface;
	cairo_t *cr;

public:
	awuiImage(int width, int height);
	awuiImage(const char * name);
	~awuiImage();

	virtual int IsClass(awuiClasses objectClass) const;

	int GetWidth();
	int GetHeight();
};
