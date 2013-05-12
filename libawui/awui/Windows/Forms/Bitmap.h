// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIBITMAP_H__
#define __AWUIBITMAP_H__

#include <awui/Windows/Forms/Control.h>

typedef unsigned int	GLuint;

namespace awui {
	namespace OpenGL {
		class GL;
	}

	namespace Windows {
		namespace Forms {
			class Bitmap : public Control {
				private:
					GLuint texture;
					int textureWidth;
					int textureHeight;
					int fixX1;
					int fixX2;
					int fixY1;
					int fixY2;

					String file;
					bool loaded;

					void Load();

				public:
					Bitmap(const String file);
					virtual ~Bitmap();

					virtual int IsClass(Classes::Enum objectClass) const;

					void SetFixedMargins(int x1, int y1, int x2, int y2);

					virtual void OnPaint(OpenGL::GL* gl);
			};
		}
	}
}

#endif
