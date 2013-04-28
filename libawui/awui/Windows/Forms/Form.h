// (c) Copyright 2011 Borja Sánchez Zamorano (BSD License)
// feedback: borsanza AT gmail DOT com

#pragma once
#ifndef __AWUIFORM_H__
#define __AWUIFORM_H__

#include <awui/Collections/ArrayList.h>
#include <awui/String.h>
#include <awui/Windows/Forms/Control.h>
#include <awui/Windows/Forms/Statistics/Stats.h>

namespace awui {
	namespace Windows {
		namespace Forms {
			class Form : public Control {
				friend class Application;
				friend class Control;

				private:
					Control * mouseControlOver;
					String text;
					Statistics::Stats stats;

					int mouseX;
					int mouseY;
					int mouseButtons;
					int fullscreen;
					int fullscreenWidth;
					int fullscreenHeight;
					int initialized;

					void OnPaintForm();

					void ProcessEvents();

				public:
					Form();
					virtual ~Form();

					virtual int IsClass(Classes::Enum objectClass) const;

					void Init();
					void SetText(String title);
					void RefreshVideo();
					void SetFullscreen(int mode);
			};
		}
	}
}

#endif
