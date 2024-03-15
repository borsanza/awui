#pragma once

#include <awui/Object.h>
#include <iterator>

namespace awui::Collections {
	class ArrayList : public Object {
	  private:
		Object **m_data;
		int m_count;
		int m_size;

	  public:
		class Iterator {
		  public:
			using iterator_category = std::input_iterator_tag;
			using value_type = Object *;
			using difference_type = std::ptrdiff_t;
			using pointer = Object **;
			using reference = Object *&;

		  private:
			pointer ptr;

		  public:
			explicit Iterator(pointer ptr) : ptr(ptr) {}

			reference operator*() const { return *ptr; }
			pointer operator->() { return ptr; }

			Iterator &operator++() {
				++ptr;
				return *this;
			}

			bool operator==(const Iterator &other) { return ptr == other.ptr; };
			bool operator!=(const Iterator &other) { return ptr != other.ptr; };
		};

		ArrayList();
		virtual ~ArrayList();

		virtual bool IsClass(Classes objectClass) const override;

		virtual String ToString() const override;

		virtual void Add(Object *item);
		inline int GetCount() const { return m_count; }

		void Clear();
		Object *Get(int index) const;
		int IndexOf(Object *item) const;
		void Remove(Object *item);
		void RemoveAt(int index);
		void SetChildIndex(Object *item, int index);
		virtual void Replace(Object *oldItem, Object *newItem);

		Iterator begin() { return Iterator(m_data); }

		Iterator end() { return Iterator(m_data + m_count); }
	};
} // namespace awui::Collections
