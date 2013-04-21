// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIRANDOM_H__
#define __AWUIRANDOM_H__

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
	};
}

#endif
