#pragma once

#include <awui/Object.h>

namespace awui {
	namespace Collections {
		class ArrayList : public Object {
			private:
				Object ** _data;
				int _count;
				int _size;

			public:
				ArrayList();
				virtual ~ArrayList();

				virtual int IsClass(Classes::Enum objectClass) const;

				virtual String ToString();

				virtual void Add(Object * item);
				inline int GetCount() const { return this->_count; }

				void Clear();
				Object * Get(int index);
				int IndexOf(Object * item);
				void Remove(Object * item);
				void RemoveAt(int index);
				void SetChildIndex(Object * item, int index);
		};
	}
}
