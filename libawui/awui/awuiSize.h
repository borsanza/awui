// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUISIZE_H__
#define __AWUISIZE_H__

class awuiSize {
private:
	int width;
	int height;

public:
	int GetWidth();
	void SetWidth(int width);

	int GetHeight();
	void SetHeight(int height);

	awuiSize & operator= (const awuiSize & other);
};

#endif