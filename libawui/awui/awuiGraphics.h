// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiObject.h"

class awuiImage;
class awuiColor;
class awuiPen;

typedef struct _cairo_surface cairo_surface_t;
typedef struct _cairo cairo_t;

class awuiGraphics : public awuiObject {
private:
	cairo_surface_t *cairo_surface;
	cairo_t *cr;

	void SetPen(awuiPen * pen);

public:
	awuiGraphics(void);
	~awuiGraphics(void);
	
	virtual int IsClass(awuiClasses objectClass);

	static awuiGraphics * FromImage(awuiImage *image);

	void Clear(awuiColor * color);
	void DrawRectangle(awuiPen * pen, float x, float y, float width, float height);
	void FillRectangle(awuiColor * color, float x, float y, float width, float height);
	void DrawImage(awuiImage * image, float x, float y);
	void DrawImage(awuiImage * image, float x, float y, float width, float height);
	void DrawLine(awuiPen * pen, float x1, float y1, float x2, float y2);
};