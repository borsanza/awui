// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIREGISTERS_H__
#define __AWUIREGISTERS_H__

namespace awui {
	namespace Emulation {
		namespace Chip8 {
			class Registers {
				private:
					unsigned char * _v;
					unsigned char _length;
					unsigned int _i;

				public:
					Registers(unsigned char n);
					~Registers();

					void SetV(unsigned char pos, unsigned char value);
					unsigned char GetV(unsigned char pos);

					void SetI(unsigned int value);
					unsigned int GetI();
			};
		}
	}
}

#endif
