// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Object.h>

typedef struct _ag ag;
class awuiForm;

using namespace awui;

class awuiApplication : public Object {
	friend class awuiForm;

private:
	static ag * g;

public:
	static void Run(awuiForm * form);

	virtual int IsClass(Classes objectClass) const;
};
