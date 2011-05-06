// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

class awuiImage;
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

	void FillRectangle(int r, int g, int b, int a, int x, int y, int width, int height);

	void DrawLine(int r, int g, int b, int a, int x, int y, int x2, int y2);
};
