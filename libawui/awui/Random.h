#pragma once

#include <cstdint>
#include <random>

namespace awui {
	class Random {
	  private:
		std::mt19937 m_rng;

	  public:
		Random(unsigned int seed = std::random_device{}())
			: m_rng(seed){};
		~Random() = default;

	  public:
		int Next();
		int Next(int min, int exclusive_max);
		int Next(int exclusive_max);
		uint8_t NextByte();
	};
} // namespace awui
