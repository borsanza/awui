// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Windows/Forms/Control.h>

typedef struct _aw aw;
typedef struct _ac ac;
typedef unsigned int GLuint;

class awuiApplication;

class awuiForm : public awui::Windows::Forms::Control {
	friend class awuiApplication;
	friend class awui::Windows::Forms::Control;

private:
	aw * w;
	Control * mouseControlOver;

	int mouseX;
	int mouseY;
	int mouseButtons;

	GLuint texture1;
	int old1w, old1h;
	GLuint texture2;
	int old2w, old2h;
	void OnPaintForm();

	void ProcessEvents(ac * c);

public:
	awuiForm();
	~awuiForm();

	virtual int IsClass(awui::Classes objectClass) const;

	void Show();
};
