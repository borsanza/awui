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
		  private:
			char m_stateSystem[4];
			uint32_t m_stateCRC;

		  protected:
			UI::Station::StationUI *m_station;

			// De quién son los estados: el sistema (cuatro letras, "SMS ", "ZX  "...) y el CRC32 del juego (la ROM o
			// la cinta). Se llama al cargar el juego
			void SetStateIdentity(const char *system, uint32_t crc);

			// CRC32 del contenido de un fichero (0 si no se puede leer)
			static uint32_t GetFileCRC32(const String &file);

			// Fichero de estado: una cabecera (marca, versión del formato, sistema, CRC del juego y tamaño) y los
			// datos. Escritura atómica (temporal y renombrar). Al leer, si la cabecera no es de este sistema y este
			// juego, o el tamaño no es el exacto, no se carga. Los estados sin cabecera (anteriores a ella) se cargan
			// si tienen el tamaño exacto. Los errores se avisan en pantalla
			bool WriteStateFile(const String &file, const uint8_t *data, int size);
			bool ReadStateFile(const String &file, uint8_t *data, int size);

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
			// La rueda no hace nada en un juego (si no, llegaría como flechas al mando del juego)
			virtual bool OnMouseWheel(UI::Events::MouseEventArgs *e) override { return true; }
		};
	} // namespace Emulators
} // namespace awui::UI
