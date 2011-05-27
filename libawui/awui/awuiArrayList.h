// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include "awuiObject.h"

class awuiArrayList : public awuiObject {
private:
	struct awuiArraListItem {
		void * object;
		awuiArraListItem * next;
	};

	awuiArraListItem * first;
	int count;

public:
	awuiArrayList();
	~awuiArrayList();

	virtual int IsClass(awuiClasses objectClass) const;

	virtual void Add(void * item);
	int GetCount();

	void Clear();
	void * Get(int index);
	int IndexOf(void * item);
	void Remove(void * item);
	void RemoveAt(int index);
	void SetChildIndex(void * item, int index);
};