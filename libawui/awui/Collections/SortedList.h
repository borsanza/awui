// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUISORTEDLIST_H__
#define __AWUISORTEDLIST_H__

#include <awui/Object.h>

namespace awui {
	namespace Collections {
		class SortedList : public Object {
			private:
				struct ArraListItem {
					Object * key;
					Object * value;
					ArraListItem * next;
				};

				ArraListItem * first;
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

#endif
