#ifndef _AWUI_EMULATION_CHIP8_STACK_H
#define _AWUI_EMULATION_CHIP8_STACK_H

#include <awui/Object.h>

namespace awui {
	namespace Collections {
		class Stack;
	}

	namespace Emulation {
		namespace Chip8 {
			class StackInt : public awui::Object {
				private:
					int _i;

				public:
					StackInt(int i) { this->_i = i; }
					int GetValue() const { return this->_i; }
			};

			class Stack {
				private:
					Collections::Stack * _stack;

				public:
					Stack();
					virtual ~Stack();

					void Push(int value);
					int Pop();
					void Clear();
			};
		}
	}
}

#endif
