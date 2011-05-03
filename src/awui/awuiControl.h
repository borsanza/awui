// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiComponent.h"

class awuiControl :	public awuiComponent {
protected:
	int x;
	int y;
	int width;
	int height;

public:
	awuiControl();
	~awuiControl();

	void SetLeft(int x);
	void SetTop(int y);
	void SetLocation(int x, int y);

	void SetWidth(int width);
	void SetHeight(int height);
	void SetSize(int width, int height);

	void SetBounds(int x, int y, int width, int height);
	
	int GetTop();
	int GetLeft();
	void GetLocation(int &x, int &y);

	int GetWidth();
	int GetHeight();
	void GetSize(int &width, int &height);

	void GetBounds(int &x, int &y, int &width, int &height);
};