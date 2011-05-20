// (c) Copyright 2011 Borja S�nchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

class awuiArrayList {
private:
	struct awuiArraListItem {
		void * object;
		awuiArraListItem * next;
	} _item;

	awuiArraListItem * first;
	int count;

public:
	awuiArrayList();
	~awuiArrayList();

	virtual void Add(void * item);
	int GetCount();

	void Clear();
	void * Get(int index);
	int IndexOf(void * item);
	void Remove(void * item);
	void RemoveAt(int index);
	void SetChildIndex(void * item, int index);
};