#pragma once

#include <awui/Object.h>

typedef struct _SDL_GameController SDL_GameController;

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
				
				Controller(SDL_GameController * controller);
				virtual ~Controller();

				void SetOrder(int position) { m_positionOrder = position; }

			public:
				int GetOrder() { return m_positionOrder; }

				static Controller * AddOnce(SDL_GameController * controller);
				static void Refresh();
				static void CloseAll();
		};
	}
}
