#pragma once

#include <awui/Windows/Forms/Control.h>

typedef struct SDL_Window SDL_Window;
typedef void * SDL_GLContext;
typedef union SDL_Event SDL_Event;

namespace awui {
	namespace Collections {
		class ArrayList;
	}

	namespace Diagnostics {
		class Process;
	}

	namespace Windows::Forms {
		class Form : public Control {
			friend class Application;
			friend class Control;

		  private:
			static ArrayList * m_formsList;
			static uint32_t m_buttonsPad1;
			static uint32_t m_buttonsPad2;
			Control * m_mouseControlOver;
			// awui::Diagnostics::Process* remoteProcess;
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
			bool m_swapInterval;

			void OnPaintForm();

		  public:
			Form();
			virtual ~Form();

			virtual bool IsClass(Classes objectClass) const override;

			void Init();
			void SetText(String title);
			void RefreshVideo();
			void SetFullscreen(int mode);
			inline int GetFullscreen() { return m_fullscreen; }

			virtual void OnRemoteHeartbeat();

			virtual void OnTick(float deltaSeconds);

			virtual bool OnRemoteKeyPress(int which, RemoteButtons::Enum button);
			virtual bool OnRemoteKeyUp(int which, RemoteButtons::Enum button);

			inline static uint32_t GetButtonsPad1() { return Form::m_buttonsPad1; }
			inline static uint32_t GetButtonsPad2() { return Form::m_buttonsPad2; }
			void SwapGL();

			uint32_t GetWindowID();
			void ProcessEvents(SDL_Event * event);

			bool SetSwapInterval(bool mode);
			bool GetSwapInterval();
		};
	} // namespace Windows::Forms
} // namespace awui
