#pragma once

#include <awui/Object.h>

namespace awui::Collections {
	class ArrayList : public Object {
		private:
			Object ** m_data;
			int m_count;
			int m_size;

		public:
			ArrayList();
			virtual ~ArrayList();

			virtual bool IsClass(Classes objectClass) const override;

			virtual String ToString() const override;

			virtual void Add(Object * item);
			inline int GetCount() const { return m_count; }

			void Clear();
			Object * Get(int index) const;
			int IndexOf(Object * item) const;
			void Remove(Object * item);
			void RemoveAt(int index);
			void SetChildIndex(Object * item, int index);
			virtual void Replace(Object * oldItem, Object * newItem);
	};
}
