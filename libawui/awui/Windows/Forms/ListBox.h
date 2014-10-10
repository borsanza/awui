// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Collections {
		class ArrayList;
	}

	namespace Windows {
		namespace Forms {
			class ObjectCollection;

			class ListBox : public Control {
				private:
					ObjectCollection * collection;

				public:
					ListBox();
					virtual ~ListBox();

					virtual int IsClass(Classes::Enum objectClass) const;

					ObjectCollection * GetItems() const;
			};

			class ObjectCollection : Object {
				private:
					ListBox * listbox;
					awui::Collections::ArrayList * arraylist;

				public:
					ObjectCollection(ListBox * owner);
					virtual ~ObjectCollection();

					int GetCount() const;

					void Add(Object * item);
					virtual void Clear();
					bool Contains(Object * value) const;
					int IndexOf(Object * value) const;
//					void Insert(int index, Object item);
					void Remove(Object * value);
					void RemoveAt(int index);
			};
		}
	}
}