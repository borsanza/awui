// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiObject.h"

typedef struct _ag ag;
class awuiForm;

class awuiApplication : public awuiObject {
	friend class awuiForm;

private:
	static ag * g;

public:
	static void Run(awuiForm * form);

	virtual int IsClass(awuiClasses objectClass) const;
};