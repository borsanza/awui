// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiControl.h"

typedef struct _aw aw;
typedef struct _ac ac;
typedef unsigned int GLuint;

class awuiApplication;

class awuiForm : public awuiControl {
	friend class awuiApplication;

private:
	aw * w;
	GLuint texture1;
	int old1w, old1h;
	GLuint texture2;
	int old2w, old2h;
	void OnPaintForm();

	void ProcessEvents(ac * c);

public:
	awuiForm();
	~awuiForm();
	
	virtual int IsClass(awuiClasses objectClass);

	void Show();
};