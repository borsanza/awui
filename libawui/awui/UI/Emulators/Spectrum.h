#pragma once

#include <set>

#include "ArcadeContainer.h"

#define TOTALSAVED 60

namespace awui {
	namespace Emulation::Spectrum {
		class Motherboard;
		class TapeCorder;
	} // namespace Emulation::Spectrum

	namespace UI::Emulators {
		class Spectrum : public ArcadeContainer {
		  private:
			awui::Emulation::Spectrum::Motherboard *m_motherboard;
			bool m_pause;
			int m_fileSlot;

			awui::Emulation::Spectrum::TapeCorder *m_tapecorder;

			uint8_t GetPad() const;

			int m_first;
			int m_last;
			double m_seconds; // Tiempo real pendiente de emular (menos de un frame salvo tras un parón)
			void CheckLimits();

			std::set<UI::Input::Keys::Enum> m_heldKeys; // Teclas del PC pulsadas que van al teclado del Spectrum
			uint32_t m_heldRemote;			   // Flechas pulsadas (teclas de cursor)

			String m_romFile; // Cinta o ROM cargada: los estados se guardan a su lado

			String GetStateFile() const;

		  public:
			virtual bool SaveAutoState() override;
			virtual bool LoadAutoState() override;

		  private:
			void DoKey(UI::Input::Keys::Enum key, bool pressed);
			void DoRemoteKey(UI::Input::RemoteButtons::Enum button, bool pressed);
			void UpdateMatrix();
			void ReleaseAllKeys();

			void SaveState();
			void LoadState();

		  public:
			Spectrum();
			virtual ~Spectrum();

			virtual int GetType() const { return Types::Spectrum; }

			void LoadRom(const String file);

			virtual void OnTick(float deltaSeconds);

			awui::Emulation::Spectrum::Motherboard *GetCPU();

			virtual void OnPaint(OpenGL::GL *gl);

			virtual bool OnKeyPress(UI::Input::Keys::Enum key);
			virtual bool OnKeyUp(UI::Input::Keys::Enum key);
			virtual bool OnRemoteKeyPress(int which, UI::Input::RemoteButtons::Enum button);
			virtual bool OnRemoteKeyUp(int which, UI::Input::RemoteButtons::Enum button);
			virtual void SetSoundEnabled(bool mode);

			awui::Emulation::Spectrum::TapeCorder *GetTapeCorder() { return m_tapecorder; }
			awui::Emulation::Spectrum::Motherboard *GetMotherboard() { return m_motherboard; }
		};
	} // namespace UI::Emulators
} // namespace awui
