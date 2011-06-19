// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUISIZE_H__
#define __AWUISIZE_H__

class awuiPoint;

class awuiSize {
private:
	int width;
	int height;

public:
	awuiSize();
	awuiSize(const awuiPoint pt);
	awuiSize(int width, int height);
	
	int GetWidth() const;
	void SetWidth(int width);

	int GetHeight() const;
	void SetHeight(int height);

	awuiSize & operator= (const awuiSize & other);
};

#endif