// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/String.h>
#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace OpenGL {
		class GL;
	}

	namespace Windows {
		namespace Forms {
			class Button : public Control {
			private:
				String text;
				int testx;
				int testy;
				int show;

			public:
				Button();
				virtual ~Button();

				virtual int IsClass(Classes::Enum objectClass) const;

				const String GetName();
				void SetText(const String str);

				virtual void OnMouseLeave();
				virtual void OnMouseDown(MouseEventArgs* e);
				virtual void OnMouseMove(MouseEventArgs* e);
				virtual void OnPaint(OpenGL::GL * gl);
			};
		}
	}
}
