#pragma once

#include <awui/UI/Button.h>
#include <awui/UI/Emulators/KeyHelp.h>

#include <cstdint>

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

		// Lo común a todos los emuladores: los estados (ranuras, partida automática), la pausa, el reinicio y sus
		// teclas, iguales en todos los sistemas:
		//
		//   F1 ayuda de las teclas, F2 guardar el estado, F3 cambiar de ranura, F4 cargar el estado, F5 pausa,
		//   F12 reiniciar
		//
		// (F10 y F11 son del formulario, y las demás F, de cada emulador). Cada emulador dice cómo se guarda, se
		// carga y se reinicia su máquina (GetStateSize, SaveStateData, LoadStateData, ResetMachine) y llama a
		// OnEmulatorKey al principio de OnKeyPress y OnKeyUp
		class ArcadeContainer : public UI::Button {
		  public:
			static constexpr int StateSlots = 10; // Ranuras 0 a 9

		  private:
			String m_gameFile; // ROM o cinta: los estados se llaman como ella (<juego>.state, <juego>.autostate)
			char m_stateSystem[4];
			uint32_t m_stateCRC;
			int m_stateSlot;
			bool m_paused;
			KeyHelp m_keyHelp;
			bool m_pausedByHelp; // La ayuda ha pausado el juego (al cerrarla se quita la pausa)

			String GetStateFile() const;

		  protected:
			UI::Station::StationUI *m_station;

			// El juego cargado: su fichero, el sistema (cuatro letras, "SMS ", "ZX  "...) y el CRC32 del juego (la ROM
			// o la cinta), que van en la cabecera de los estados. Se llama al cargar el juego
			void SetGame(const String &file, const char *system, uint32_t crc);

			// CRC32 del contenido de un fichero (0 si no se puede leer)
			static uint32_t GetFileCRC32(const String &file);

			// Fichero de estado: una cabecera (marca, versión del formato, sistema, CRC del juego y tamaño) y los
			// datos. Escritura atómica (temporal y renombrar). Al leer, si la cabecera no es de este sistema y este
			// juego, o el tamaño no es el exacto, no se carga. Los estados sin cabecera (anteriores a ella) se cargan
			// si tienen el tamaño exacto. Los errores se avisan en pantalla
			bool WriteStateFile(const String &file, const uint8_t *data, int size);
			bool ReadStateFile(const String &file, uint8_t *data, int size);

			// Lo que cada emulador sabe hacer. Sin estados (GetStateSize 0), F2/F3/F4 y la partida automática no
			// hacen nada
			virtual int GetStateSize() const { return 0; }
			virtual void SaveStateData(uint8_t *data) {}
			virtual void LoadStateData(uint8_t *data) {} // Con lo que haga falta después (soltar teclas...)
			virtual void ResetMachine() {}

			// Las teclas propias del sistema, para la ayuda (F1): el nombre del sistema y sus filas
			virtual KeyHelp::Section GetSystemKeys() const = 0;

			// Las teclas comunes (ver arriba). Devuelve true si era una de ellas
			bool OnEmulatorKey(UI::Input::Keys::Enum key, bool pressed);

			// En pausa (F5) el emulador no avanza: cada uno lo mira en su OnTick
			inline bool IsPaused() const { return m_paused; }

		  public:
			ArcadeContainer();
			virtual ~ArcadeContainer() = default;

			virtual void SetSoundEnabled(bool mode) {}

			// Estados en la ranura actual
			void SaveState();
			void LoadState();
			void SetStateSlot(int slot);
			inline int GetStateSlot() const { return m_stateSlot; }

			void SetPaused(bool paused);
			void Reset();

			// Ayuda de las teclas (F1). Mientras se ve, el juego está en pausa; cualquier tecla la cierra
			void ShowKeyHelp();
			void HideKeyHelp();

			// Partida automática (<juego>.autostate): se guarda al salir del juego y se recupera al volver a entrar.
			// Los emuladores que no tienen estados no hacen nada
			bool SaveAutoState();
			bool LoadAutoState();

			virtual void SetDebugger(DebuggerSMS *debugger){};
			virtual int GetType() const = 0;

			void SetStationUI(UI::Station::StationUI *station);

			virtual bool OnRemoteKeyUp(int which, UI::Input::RemoteButtons::Enum button);
			// La rueda no hace nada en un juego (si no, llegaría como flechas al mando del juego)
			virtual bool OnMouseWheel(UI::Events::MouseEventArgs *e) override { return true; }
		};
	} // namespace Emulators
} // namespace awui::UI
