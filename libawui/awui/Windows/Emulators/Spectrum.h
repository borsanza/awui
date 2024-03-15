#pragma once

#include "ArcadeContainer.h"

#define TOTALSAVED 60

namespace awui {
	namespace Emulation::Spectrum {
		class Motherboard;
		class TapeCorder;
	} // namespace Emulation::Spectrum

	namespace Windows::Emulators {
		class Spectrum : public ArcadeContainer {
		  private:
			awui::Emulation::Spectrum::Motherboard *m_motherboard;
			bool m_pause;
			int m_fileSlot;

			awui::Emulation::Spectrum::TapeCorder *m_tapecorder;

			uint8_t GetPad() const;

			int m_first;
			int m_last;
			long long m_lastTick;
			void CheckLimits();

			void CallKey(int key, bool pressed);
			void DoKey(Keys::Enum key, bool pressed);
			void DoRemoteKey(RemoteButtons::Enum button, bool pressed);

			void SaveState();
			void LoadState();

		  public:
			Spectrum();
			virtual ~Spectrum();

			virtual bool IsClass(Classes objectClass) const override;
			virtual int GetType() const { return Types::Spectrum; }

			void LoadRom(const String file);

			virtual void OnTick(float deltaSeconds);

			awui::Emulation::Spectrum::Motherboard *GetCPU();

			virtual void OnPaint(OpenGL::GL *gl);

			virtual bool OnKeyPress(Keys::Enum key);
			virtual bool OnKeyUp(Keys::Enum key);
			virtual bool OnRemoteKeyPress(int which, RemoteButtons::Enum button);
			virtual bool OnRemoteKeyUp(int which, RemoteButtons::Enum button);
			virtual void SetSoundEnabled(bool mode);

			awui::Emulation::Spectrum::TapeCorder *GetTapeCorder() { return this->m_tapecorder; }
			awui::Emulation::Spectrum::Motherboard *GetMotherboard() { return this->m_motherboard; }
		};
	} // namespace Windows::Emulators
} // namespace awui
