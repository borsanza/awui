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

		inline const std::vector<int> &GetValues() const { return m_stack; }
		inline void SetValues(const std::vector<int> &values) { m_stack = values; }
	};
} // namespace awui::Emulation::Chip8
