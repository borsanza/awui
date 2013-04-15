// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIFORM_H__
#define __AWUIFORM_H__

#include <awui/Windows/Forms/Control.h>
#include <awui/Collections/ArrayList.h>

typedef struct _aw aw;
typedef struct _ac ac;
typedef unsigned int GLuint;

namespace awui {
	namespace Windows {
		namespace Forms {
			class Form : public Control {
				friend class Application;
				friend class Control;

			private:
				aw * w;
				Control * mouseControlOver;

				int mouseX;
				int mouseY;
				int mouseButtons;

				GLuint texture1;
				int old1w, old1h;
				GLuint texture2;
				int old2w, old2h;
				void OnPaintForm();

				void ProcessEvents(ac * c);

			public:
				Form();
				virtual ~Form();

				virtual int IsClass(Classes::Enum objectClass) const;

				void Show();
			};
		}
	}
}

#endif
