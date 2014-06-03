// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUISCREEN_H__
#define __AWUISCREEN_H__

namespace awui {
	namespace Emulation {
		namespace Chip8 {
			class Screen {
				private:
					int _width;
					int _height;
					unsigned char * _data;

				public:
					Screen(int width, int height);
					~Screen();

					void Clear();
			};
		}
	}
}

#endif
