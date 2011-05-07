// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

typedef struct _ag ag;
class awuiForm;

class awuiApplication {
	friend class awuiForm;

private:
	static ag * g;

public:
	static void Run(awuiForm * form);
};