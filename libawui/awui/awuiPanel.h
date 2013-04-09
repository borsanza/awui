// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiControl.h"

class awuiPanel : public awuiControl {
public:
	awuiPanel();
	~awuiPanel();

	virtual int IsClass(Classes objectClass) const;

	const virtual Size GetMinimumSize() const;
};
