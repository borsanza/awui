#ifndef _AWUI_WINDOWS_FORMS_FORM_H
#define _AWUI_WINDOWS_FORMS_FORM_H

#include <awui/String.h>
#include <awui/Windows/Forms/Control.h>
#include <awui/Windows/Forms/RemoteButtons.h>

namespace awui {
	namespace Diagnostics {
		class Process;
	}

	namespace Windows {
		namespace Forms {
			class Bitmap;
			class Form : public Control {
				friend class Application;
				friend class Control;

				private:
					static Bitmap * selectedBitmap;
					static Control * controlSelected;
					Control * mouseControlOver;
					awui::Diagnostics::Process* remoteProcess;
					String text;
					uint32_t _buttons;

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

					static Control * GetControlSelected();
					static void SetControlSelected(Control * selected);
					static Bitmap * GetSelectedBitmap();
					virtual void OnRemoteHeartbeat();

					virtual void OnTick();

					virtual void OnRemoteKeyPressPre(RemoteButtons::Enum button);
					virtual void OnRemoteKeyUpPre(RemoteButtons::Enum button);
			};
		}
	}
}

#endif
