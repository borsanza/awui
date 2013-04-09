// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Collections/ArrayList.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class Control;
		}
	}
}

class awuiControlCollection : public awui::Collections::ArrayList {
private:
	awui::Windows::Forms::Control * owner;

public:
	awuiControlCollection(awui::Windows::Forms::Control *);
	~awuiControlCollection();

	virtual int IsClass(awui::Classes objectClass) const;

	awui::Windows::Forms::Control * GetOwner();
	virtual void Add(void * item);
};
