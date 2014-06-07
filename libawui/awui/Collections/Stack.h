#ifndef _AWUI_COLLECTIONS_STACK_H
#define _AWUI_COLLECTIONS_STACK_H

#include <awui/Object.h>

namespace awui {
	namespace Collections {
		class Stack : public Object {
			private:
				struct StackItem {
					Object * object;
					StackItem * prev;
				};

				StackItem * _last;
				int _count;

			public:
				Stack();
				virtual ~Stack();

				virtual int IsClass(Classes::Enum objectClass) const;

				virtual String ToString();

				virtual void Push(Object * item);
				int GetCount();
				void Clear();
				Object * Pop();
		};
	}
}

#endif