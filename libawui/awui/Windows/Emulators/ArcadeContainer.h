#pragma once

#include <awui/Windows/Forms/Button.h>

using namespace awui::Windows::Forms;

namespace awui::Windows {
	namespace Forms::Station {
		class StationUI;
	}

	using namespace awui::Windows::Forms::Station;

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

		class ArcadeContainer : public Button {
			protected:
				StationUI * m_station;

			public:
				ArcadeContainer();
				virtual ~ArcadeContainer() = default;

				virtual bool IsClass(Classes objectClass) const override;

				virtual void SetSoundEnabled(bool mode) {}
				virtual void SetDebugger(DebuggerSMS * debugger) {};
				virtual int GetType() const = 0;

				void SetStationUI(StationUI * station);

				virtual bool OnRemoteKeyUp(int which, RemoteButtons::Enum button);
		};
	}
}
