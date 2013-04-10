// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#include <awui/Windows/Forms/Control.h>

namespace awui {
	namespace OpenGL {
		class GL;
	}

	namespace Windows {
		namespace Forms {
			class Button : public Control {
			private:
				std::string text;
				int testx;
				int testy;
				int show;

			public:
				Button();
				~Button();

				virtual int IsClass(Classes::Enum objectClass) const;

				const std::string& GetName();
				void SetText(const std::string& str);

				virtual void OnMouseLeave();
				virtual void OnMouseDown(MouseEventArgs* e);
				virtual void OnMouseMove(MouseEventArgs* e);
				virtual void OnPaint(OpenGL::GL * gl);
			};
		}
	}
}
