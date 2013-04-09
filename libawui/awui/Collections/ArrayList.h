// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Object.h>

namespace awui {
	namespace Collections {
		class ArrayList : public Object {
		private:
			struct ArraListItem {
				void * object;
				ArraListItem * next;
			};

			ArraListItem * first;
			int count;

		public:
			ArrayList();
			~ArrayList();

			virtual int IsClass(Classes objectClass) const;

			virtual void Add(void * item);
			int GetCount();

			void Clear();
			void * Get(int index);
			int IndexOf(void * item);
			void Remove(void * item);
			void RemoveAt(int index);
			void SetChildIndex(void * item, int index);
		};
	}
}
