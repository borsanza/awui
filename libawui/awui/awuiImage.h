// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

class awuiImage {
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

	int GetWidth();
	int GetHeight();
};