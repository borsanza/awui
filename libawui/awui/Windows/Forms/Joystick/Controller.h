#pragma once

#include <awui/Object.h>

typedef struct _SDL_GameController SDL_GameController;

namespace awui::Windows::Forms::Joystick {
	class Controller : public Object {
		private:
			SDL_GameController * m_controller;

		public:
			Controller(SDL_GameController * controller);
			virtual ~Controller();

	}
}
