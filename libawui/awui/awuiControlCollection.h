// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiArrayList.h"

class awuiControl;

class awuiControlCollection : public awuiArrayList {
private:
	awuiControl * owner;
	
public:
  awuiControlCollection(awuiControl *);
  ~awuiControlCollection();

	virtual int IsClass(awuiClasses objectClass);

	awuiControl * GetOwner();
	virtual void Add(void * item);
};