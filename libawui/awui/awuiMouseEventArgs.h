// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Object.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class Control;
		}
	}
}

class awuiMouseEventArgs : public awui::Object {
private:
	friend class awui::Windows::Forms::Control;

	int x, y;
	int delta;
	int clicks;
	int button;

private:
	void SetX(int x);
	void SetY(int y);
	void SetLocation(int x, int y);
	void SetDelta(int delta);
	void SetClicks(int clicks);
	void SetButton(int button);

public:
  awuiMouseEventArgs();
  ~awuiMouseEventArgs();

	virtual int IsClass(awui::Classes objectClass) const;

	int GetX();
	int GetY();
	void GetLocation(int &x, int &y);
	int GetDelta();
	int GetClicks();
	int GetButton();
};
