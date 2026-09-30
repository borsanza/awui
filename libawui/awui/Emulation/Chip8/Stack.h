#pragma once

#include <vector>

namespace awui::Emulation::Chip8 {
	// Pila de direcciones de retorno (CALL / RET)
	class Stack {
	  private:
		std::vector<int> m_stack;

	  public:
		void Push(int value);
		int Pop();
		void Clear();
	};
} // namespace awui::Emulation::Chip8
