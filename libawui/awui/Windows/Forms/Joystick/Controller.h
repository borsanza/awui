#pragma once

#include <awui/Object.h>

typedef struct _SDL_GameController SDL_GameController;
typedef int32_t Sint32;
typedef Sint32 SDL_JoystickID;

namespace awui {
	namespace Collections {
		class ArrayList;
	}

	namespace Windows::Forms::Joystick {
		class Controller : public Object {
			private:
				static Collections::ArrayList * m_controllersList;
				SDL_GameController * m_controller;
				int m_positionOrder;
				SDL_JoystickID m_which;
				uint32_t m_buttons;
				uint32_t m_prevButtons;
				
				Controller(SDL_GameController * controller);
				virtual ~Controller();

				void SetOrder(int position) { m_positionOrder = position; }

			public:
				int GetOrder() const { return m_positionOrder; }
				uint32_t GetButtons() const { return m_buttons; }
				uint32_t GetPrevButtons() const { return m_prevButtons; }

				static Controller * AddOnce(SDL_GameController * controller);
				static Controller * GetByWhich(SDL_JoystickID which);
				static void Refresh();
				static void CloseAll();

				void OnButtonDown(uint32_t button);
				void OnButtonUp(uint32_t button);
		};
	}
}
