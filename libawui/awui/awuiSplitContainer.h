// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#ifndef __AWUISPLITCONTAINER_H__
#define __AWUISPLITCONTAINER_H__

#include <awui/Windows/Forms/Control.h>

class awuiSplitter;
class awuiPanel;

class awuiSplitContainer : public awui::Windows::Forms::Control {
public:
	enum Orientation {
		Vertical,
		Horizontal,
	};

private:
	awuiSplitter * splitter;
	awuiPanel * panel1;
	awuiPanel * panel2;

	float splitterDistance;
	int splitterIncrement;
	int splitterWidth;
	Orientation orientation;

	void RecalculatePositions();

	void SetLocationW(Control * control, int pos);
	void SetSizeW(Control* control, int size, int substract);
	int GetSizeW(Control * control) const;
	int GetMinimumSizeW(Control * control) const;

public:
	awuiSplitContainer();
	~awuiSplitContainer();

	virtual int IsClass(awui::Classes objectClass) const;

	awuiPanel * GetPanel1();
	awuiPanel * GetPanel2();

	float GetSplitterDistance() const;
	void SetSplitterDistance(int distance);

	int GetSplitterIncrement() const;
	void SetSplitterIncrement(int increment);

	int GetSplitterWidth() const;
	void SetSplitterWidth(int width);

	Orientation GetOrientation() const;
	void SetOrientation(Orientation orientation);

	virtual void Layout();
};

#endif
