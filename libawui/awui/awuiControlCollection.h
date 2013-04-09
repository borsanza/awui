// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Collections/ArrayList.h>

class awuiControl;
using namespace awui;

class awuiControlCollection : public awui::Collections::ArrayList {
private:
	awuiControl * owner;

public:
	awuiControlCollection(awuiControl *);
	~awuiControlCollection();

	virtual int IsClass(Classes objectClass) const;

	awuiControl * GetOwner();
	virtual void Add(void * item);
};
