#pragma once

#include <awui/Windows/Forms/Button.h>

namespace awui::Windows {
	namespace Forms::Station {
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

		class ArcadeContainer : public Forms::Button {
		  protected:
			Forms::Station::StationUI *m_station;

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

			void SetStationUI(Forms::Station::StationUI *station);

			virtual bool OnRemoteKeyUp(int which, Forms::Input::RemoteButtons::Enum button);
		};
	} // namespace Emulators
} // namespace awui::Windows
