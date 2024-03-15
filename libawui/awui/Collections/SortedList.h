#pragma once

#include <awui/Object.h>

namespace awui::Collections {
	class SortedList : public Object {
	  private:
		struct SortedListItem {
			Object *key;
			Object *value;
			SortedListItem *next;
		};

		SortedListItem *m_first;
		SortedListItem *m_last;
		int m_count;

	  public:
		SortedList();
		virtual ~SortedList();

		virtual bool IsClass(Classes objectClass) const override;

		virtual String ToString() const override;

		virtual void Add(Object *key, Object *value);
		int GetCount();

		void Clear();
		Object *GetKey(int index);
		Object *GetByIndex(int index);
		void RemoveAt(int index);
	};
} // namespace awui::Collections
