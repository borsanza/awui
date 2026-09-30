#pragma once

#include <awui/UI/Button.h>

namespace awui::UI {
	namespace Station {
		class StationUI;
	}

	namespace Emulators {
		struct Types {
			enum Enum {
				Undefined = 0,
				Chip8,
				MasterSystem,
				GameGear,
				Spectrum,
			};
		};

		class DebuggerSMS;

		class ArcadeContainer : public UI::Button {
		  protected:
			UI::Station::StationUI *m_station;

			// Fichero de estado: escritura atómica (temporal y renombrar) y lectura que exige el tamaño exacto
			static bool WriteStateFile(const String &file, const uint8_t *data, int size);
			static bool ReadStateFile(const String &file, uint8_t *data, int size);

		  public:
			ArcadeContainer();
			virtual ~ArcadeContainer() = default;

			virtual void SetSoundEnabled(bool mode) {}

			// Partida automática (junto a la ROM, <rom>.autostate): se guarda al salir del juego y se recupera al
			// volver a entrar. Los emuladores que no tienen estados no hacen nada
			virtual bool SaveAutoState() { return false; }
			virtual bool LoadAutoState() { return false; }
			virtual void SetDebugger(DebuggerSMS *debugger){};
			virtual int GetType() const = 0;

			void SetStationUI(UI::Station::StationUI *station);

			virtual bool OnRemoteKeyUp(int which, UI::Input::RemoteButtons::Enum button);
		};
	} // namespace Emulators
} // namespace awui::UI
