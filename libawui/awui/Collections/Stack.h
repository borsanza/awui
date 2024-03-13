#pragma once

#include <awui/Object.h>

namespace awui {
	namespace Collections {
		class Stack : public Object {
			private:
				struct StackItem {
					Object * object;
					StackItem * prev;
				};

				StackItem * m_last;
				int m_count;

			public:
				Stack();
				virtual ~Stack();

				virtual bool IsClass(Classes objectClass) const override;

				virtual String ToString() const override;

				virtual void Push(Object * item);
				int GetCount();
				void Clear();
				Object * Pop();
		};
	}
}
