// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUIPOINT_H__
#define __AWUIPOINT_H__

class awuiSize;

class awuiPoint {
private:
	int x;
	int y;

public:
	awuiPoint();
	awuiPoint(const awuiSize sz);
	awuiPoint(int x, int y);

	int GetX() const;
	void SetX(int x);

	int GetY() const;
	void SetY(int y);

	awuiPoint & operator= (const awuiPoint & other);
};

#endif