// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUIPOINT_H__
#define __AWUIPOINT_H__

class awuiPoint {
private:
	int x;
	int y;

public:
	int GetX();
	void SetX(int x);

	int GetY();
	void SetY(int y);

	awuiPoint & operator= (const awuiPoint & other);
};

#endif