// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

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
