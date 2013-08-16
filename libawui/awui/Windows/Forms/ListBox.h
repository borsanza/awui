// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class ListBox : public Control {
				public:
					ListBox();
					virtual ~ListBox();

					virtual int IsClass(Classes::Enum objectClass) const;
			};

			class ObjectCollection {
				public:
            ObjectCollection(ListBox owner);
            int GetCount();
//            public virtual object this[int index] { get; set; }
            int Add(Object item);
            virtual void Clear();
            bool Contains(Object value);
            int IndexOf(Object value);
            void Insert(int index, Object item);
            void Remove(Object value);
            void RemoveAt(int index);
			};
		}
	}
}
