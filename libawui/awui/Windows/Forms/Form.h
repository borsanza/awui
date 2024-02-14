#pragma once

#include <awui/Windows/Forms/Control.h>

typedef struct SDL_Window SDL_Window;
typedef void *SDL_GLContext;

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
					static Bitmap * m_selectedBitmap;
					static Control * m_controlSelected;
					static uint32_t m_buttonsPad1;
					static uint32_t m_buttonsPad2;
					Control * m_mouseControlOver;
					//awui::Diagnostics::Process* remoteProcess;
					String m_text;
					SDL_Window * m_window;
					SDL_GLContext m_context;

					int m_mouseX;
					int m_mouseY;
					int m_mouseButtons;
					int m_initialized;

					int m_fullscreen;
					int m_lastFullscreenState;
					int m_lastWidth;
					int m_lastHeight;

					void OnPaintForm();

					void ProcessEvents();

				public:
					Form();
					virtual ~Form();

					virtual bool IsClass(Classes objectClass) const override;

					void Init();
					void SetText(String title);
					void RefreshVideo();
					void SetFullscreen(int mode);
					inline int GetFullscreen() { return m_fullscreen; }

					static Control * GetControlSelected();
					static void SetControlSelected(Control * selected);
					static Bitmap * GetSelectedBitmap();
					virtual void OnRemoteHeartbeat();

					virtual void OnTick();

					virtual bool OnRemoteKeyPress(int which, RemoteButtons::Enum button);
					virtual bool OnRemoteKeyUp(int which, RemoteButtons::Enum button);

					inline static uint32_t GetButtonsPad1() { return Form::m_buttonsPad1; }
					inline static uint32_t GetButtonsPad2() { return Form::m_buttonsPad2; }
					void SwapGL();
			};
		}
	}
}

