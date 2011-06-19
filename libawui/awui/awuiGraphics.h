// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiObject.h"
#include <string>

class awuiImage;
class awuiColor;
class awuiPen;
class awuiFont;
class awuiSize;

typedef struct _cairo_surface cairo_surface_t;
typedef struct _cairo cairo_t;

class awuiGraphics : public awuiObject {
private:
	cairo_surface_t *cairo_surface;
	cairo_t *cr;

	void SetPen(awuiPen * pen);
	awuiGraphics();

public:
	~awuiGraphics();
	
	virtual int IsClass(awuiClasses objectClass) const;

	static awuiGraphics * FromImage(awuiImage *image);

	void Clear(const awuiColor color);
	void DrawRectangle(awuiPen * pen, float x, float y, float width, float height);
	void FillRectangle(const awuiColor color, float x, float y, float width, float height);
	void DrawImage(awuiImage * image, float x, float y);
	void DrawImage(awuiImage * image, float x, float y, float width, float height);
	void DrawLine(awuiPen * pen, float x1, float y1, float x2, float y2);
	
	awuiSize GetMeasureText(const std::string& text, awuiFont *font) const;
	void DrawString(const std::string& text, awuiFont * font, float x, float y);
};