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
			bool m_resetHeld; // La combinación de reinicio sigue pulsada (el aviso sale una sola vez)

			awui::Emulation::Spectrum::TapeCorder *m_tapecorder;

			uint8_t GetPad() const;

			int m_first;
			int m_last;
			double m_seconds; // Tiempo real pendiente de emular (menos de un frame salvo tras un parón)
			bool m_fastDone;  // Modo rápido (F8): ya se ha emulado en este tick
			void CheckLimits();

			std::set<UI::Input::Keys::Enum> m_heldKeys; // Teclas del PC pulsadas que van al teclado del Spectrum
			uint32_t m_heldRemote;			   // Flechas pulsadas (teclas de cursor)

			String m_romFile; // Cinta o ROM cargada

			void DoKey(UI::Input::Keys::Enum key, bool pressed);
			void DoRemoteKey(UI::Input::RemoteButtons::Enum button, bool pressed);
			void UpdateMatrix();
			void ReleaseAllKeys();

		  protected:
			virtual int GetStateSize() const override;
			virtual void SaveStateData(uint8_t *data) override;
			virtual void LoadStateData(uint8_t *data) override;
			virtual void ResetMachine() override;
			virtual KeyHelp::Section GetSystemKeys() const override;
			virtual void EmulateTime(float seconds) override;
			virtual void RewindFrame(uint8_t *data) override;
			virtual float GetFrameSeconds() const override;
			virtual void SetSoundMode(bool reverse, bool fastForward) override;

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
			virtual void SetRewinding(bool mode) override;

			awui::Emulation::Spectrum::TapeCorder *GetTapeCorder() { return m_tapecorder; }
			awui::Emulation::Spectrum::Motherboard *GetMotherboard() { return m_motherboard; }
		};
	} // namespace UI::Emulators
} // namespace awui
