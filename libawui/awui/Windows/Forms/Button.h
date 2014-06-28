#ifndef _AWUI_WINDOWS_FORMS_BUTTON_H
#define _AWUI_WINDOWS_FORMS_BUTTON_H

#include <awui/String.h>
#include <awui/Windows/Forms/Control.h>
#include <awui/Windows/Forms/Label.h>

namespace awui {
	namespace OpenGL {
		class GL;
	}

	namespace Windows {
		namespace Forms {
			class Button : public Control {
				private:
					Label label;
					String text;

				public:
					Button();
					virtual ~Button();

					virtual int IsClass(Classes::Enum objectClass) const;

					const String GetText();
					void SetText(const String str);

					virtual void OnMouseLeave();
					virtual void OnMouseDown(MouseEventArgs* e);
					virtual void OnMouseMove(MouseEventArgs* e);
					virtual void OnPaint(OpenGL::GL * gl);
					virtual void SetForeColor(const Drawing::Color color);
					virtual void SetFont(const Drawing::Font * font);
			};
		}
	}
}

#endif