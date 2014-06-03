// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIGRAPHICS_H__
#define __AWUIGRAPHICS_H__

namespace awui {
	namespace Emulation {
		namespace Chip8 {
			class Graphics {
				private:
					int _width;
					int _height;
					unsigned char * _data;

				public:
					Graphics(int width, int height);
					~Graphics();

					void Clear();
			};
		}
	}
}

#endif
