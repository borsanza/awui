// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIARRAYLIST_H__
#define __AWUIARRAYLIST_H__

#include <awui/Object.h>

namespace awui {
	namespace Collections {
		class ArrayList : public Object {
			private:
				struct ArrayListItem {
					Object * object;
					ArrayListItem * next;
				};

				ArrayListItem * first;
				int count;

			public:
				ArrayList();
				virtual ~ArrayList();

				virtual int IsClass(Classes::Enum objectClass) const;

				virtual String ToString();

				virtual void Add(Object * item);
				int GetCount();

				void Clear();
				Object * Get(int index);
				int IndexOf(Object * item);
				void Remove(Object * item);
				void RemoveAt(int index);
				void SetChildIndex(Object * item, int index);
		};
	}
}

#endif
