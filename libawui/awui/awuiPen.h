// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUIPEN_H__
#define __AWUIPEN_H__

class awuiColor;

class awuiCap {
public:
	enum Cap {
		Butt,
		Round,
		Square
	};
};

class awuiLineJoin {
public:
	enum LineJoin {
		Miter,
		Round,
		Bevel
	};
};

class awuiPen {
private:
	awuiColor * color;
	float width;
	awuiCap::Cap cap;
	awuiLineJoin::LineJoin lineJoin;

public:
	awuiPen(awuiColor * color);
	awuiPen(awuiColor * color, float width);
	~awuiPen();

	awuiColor * GetColor();
	void SetColor(awuiColor * color);

	float GetWidth();
	void SetWidth(float width);

	void SetLineJoin(awuiLineJoin::LineJoin lineJoin);
	awuiLineJoin::LineJoin GetLineJoin();

	void SetCap(awuiCap::Cap lineCap);
	awuiCap::Cap GetCap();
};

#endif