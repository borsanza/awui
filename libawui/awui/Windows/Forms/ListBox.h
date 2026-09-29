#pragma once

#include <awui/Windows/Forms/Control.h>
#include <vector>

namespace awui {

	namespace Windows::Forms {
		class ObjectCollection;

		class ListBox : public Control {
		  private:
			ObjectCollection *m_collection;

		  public:
			ListBox();
			virtual ~ListBox();

			ObjectCollection *GetItems() const;
		};

		class ObjectCollection : Object {
		  private:
			ListBox *listbox;
			std::vector<Object *> m_items;

		  public:
			ObjectCollection(ListBox *owner);
			virtual ~ObjectCollection();

			int GetCount() const;

			void Add(Object *item);
			virtual void Clear();
			bool Contains(Object *value) const;
			int IndexOf(Object *value) const;
			//				void Insert(int index, Object item);
			void Remove(Object *value);
			void RemoveAt(int index);
		};
	} // namespace Windows::Forms
} // namespace awui
