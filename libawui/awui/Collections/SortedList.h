#pragma once

#include <awui/Object.h>
#include <awui/String.h>

namespace awui::Collections {
	// Lista ordenada por una clave de texto (se guarda una copia). Los valores no son suyos: no los borra
	class SortedList : public Object {
	  private:
		struct SortedListItem {
			String key;
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

		virtual void Add(const String &key, Object *value);
		int GetCount();

		void Clear();
		const String *GetKey(int index);
		Object *GetByIndex(int index);
		void RemoveAt(int index);
	};
} // namespace awui::Collections
