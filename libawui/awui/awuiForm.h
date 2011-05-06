// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiControl.h"

typedef struct _aw aw;
typedef struct _ac ac;

class awuiApplication;
class awuiGraphics;

class awuiForm : public awuiControl {
	friend class awuiApplication;

private:
	int red, green, blue;
	aw * w;
	void OnPaintPre();
	void OnResizePre();

	void ProcessEvents(ac * c);

public:
	awuiForm();
	~awuiForm();

	void Show();
	virtual void OnPaint(awuiGraphics * g) {}
	virtual void OnResize() {}

	void SetBackColor(int red, int green, int blue);
	void GetBackColor(int &red, int &green, int &blue);
};