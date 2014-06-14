#ifndef _AWUI_RANDOM_H
#define _AWUI_RANDOM_H

namespace awui {
	class Random {
		private:
			unsigned int seed;

		public:
			Random();
			Random(unsigned int seed);
			virtual ~Random();

		public:
			int Next();
			int Next(int exclusive_max);
			int Next(int min, int exclusive_max);
			int NextByte();
	};
}

#endif