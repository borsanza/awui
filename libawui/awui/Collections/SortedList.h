#pragma once

#include <awui/Object.h>

namespace awui {
	namespace Collections {
		class SortedList : public Object {
			private:
				struct SortedListItem {
					Object * key;
					Object * value;
					SortedListItem * next;
				};

				SortedListItem * first;
				SortedListItem * last;
				int count;

			public:
				SortedList();
				virtual ~SortedList();

				virtual int IsClass(Classes::Enum objectClass) const;

				virtual String ToString();

				virtual void Add(Object * key, Object * value);
				int GetCount();

				void Clear();
				Object * GetKey(int index);
				Object * GetByIndex(int index);
				void RemoveAt(int index);
		};
	}
}
