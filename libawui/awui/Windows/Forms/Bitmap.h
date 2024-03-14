#pragma once

#include <awui/Collections/ArrayList.h>
#include <awui/Drawing/ColorF.h>
#include <awui/Windows/Forms/Control.h>

typedef unsigned int GLuint;

namespace awui {
	namespace OpenGL {
		class GL;
	}

	namespace Windows::Forms {
		struct StretchMode {
			enum Enum {
				NoResize = 0,
				Tile = 1,
				Stretch = 2,
				AspectFit = 3,
				AspectFill = 4,
			};
		};
		class Bitmap : public Control {
		  private:
			static Collections::ArrayList list;
			GLuint m_texture;
			int m_textureWidth;
			int m_textureHeight;
			int m_fixX1;
			int m_fixX2;
			int m_fixY1;
			int m_fixY2;
			StretchMode::Enum m_stretchMode;
			Drawing::ColorF m_color;

			String m_file;
			bool m_loaded;

			void Load();
			void Unload();

			void PaintNoResized();
			void PaintTiled();
			void PaintTexture(int x, int y, int width, int height);
			void PaintStretched();
			void PaintAspectFit();
			void PaintAspectFill();

		  public:
			Bitmap(const String file);
			virtual ~Bitmap();

			virtual bool IsClass(Classes objectClass) const override;

			void SetFixedMargins(int x1, int y1, int x2, int y2);
			void GetFixedMargins(int * x1, int * y1, int * x2, int * y2);

			virtual void OnPaint(OpenGL::GL * gl);

			void SetColor(Drawing::ColorF color);

			void SetStretchMode(StretchMode::Enum stretchMode);
			StretchMode::Enum GetStretchMode();

			static void UnloadAll();
		};
	} // namespace Windows::Forms
} // namespace awui
