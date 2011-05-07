// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

class awuiImage;
class awuiColor;
class awuiPen;

typedef struct _cairo_surface cairo_surface_t;
typedef struct _cairo cairo_t;

class awuiGraphics {
private:
	cairo_surface_t *cairo_surface;
	cairo_t *cr;
public:
	awuiGraphics(void);
	~awuiGraphics(void);

	static awuiGraphics * FromImage(awuiImage *image);

	void FillRectangle(awuiColor * color, int x, int y, int width, int height);

	void DrawLine(awuiPen * pen, float x1, float y1, float x2, float y2);
};
