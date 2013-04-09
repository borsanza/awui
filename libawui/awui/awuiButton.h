// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Windows/Forms/Control.h>

class awuiButton : public awui::Windows::Forms::Control {
private:
	std::string text;
	int testx;
	int testy;
	int show;

public:
	awuiButton();
	~awuiButton();

	virtual int IsClass(awui::Classes objectClass) const;

	const std::string& GetName();
	void SetText(const std::string& str);

	//	float GetAnimationValue();

	virtual void OnMouseLeave();
	virtual void OnMouseDown(awuiMouseEventArgs* e);
	virtual void OnMouseMove(awuiMouseEventArgs* e);
	virtual void OnPaint(awuiGL * gl);
};
