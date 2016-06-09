#pragma once

#include <awui/Windows/Forms/Control.h>
#include <SDL2/SDL_video.h>

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
					static uint32_t _buttonsPad1;
					static uint32_t _buttonsPad2;
					Control * mouseControlOver;
					awui::Diagnostics::Process* remoteProcess;
					String text;
					SDL_Window * _win;

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
					inline int GetFullscreen() { return this->fullscreen; }

					static Control * GetControlSelected();
					static void SetControlSelected(Control * selected);
					static Bitmap * GetSelectedBitmap();
					virtual void OnRemoteHeartbeat();

					virtual void OnTick();

					virtual bool OnRemoteKeyPress(int which, RemoteButtons::Enum button);
					virtual bool OnRemoteKeyUp(int which, RemoteButtons::Enum button);

					inline static uint32_t GetButtonsPad1() { return Form::_buttonsPad1; }
					inline static uint32_t GetButtonsPad2() { return Form::_buttonsPad2; }
					void SwapGL();
			};
		}
	}
}
