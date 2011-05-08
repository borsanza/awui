// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiControl.h"

typedef struct _aw aw;
typedef struct _ac ac;

class awuiApplication;

class awuiForm : public awuiControl {
	friend class awuiApplication;

private:
	aw * w;
	void OnPaintPre();

	void ProcessEvents(ac * c);

public:
	awuiForm();
	~awuiForm();

	void Show();
	virtual void OnResize() {}
};