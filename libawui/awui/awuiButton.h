// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiControl.h"

class awuiButton : public awuiControl {
private:
	std::string text;
	int testx;
	int testy;
	int show;

public:
	awuiButton();
	~awuiButton();

	virtual int IsClass(awuiClasses objectClass) const;

	const std::string& GetName();
	void SetText(const std::string& str);

	//	float GetAnimationValue();

	virtual void OnMouseLeave();
	virtual void OnMouseDown(awuiMouseEventArgs* e);
	virtual void OnMouseMove(awuiMouseEventArgs* e);
	virtual void OnPaint(awuiGL * gl);
};