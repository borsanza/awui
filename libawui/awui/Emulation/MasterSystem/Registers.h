#ifndef _AWUI_EMULATION_MASTERSYSTEM_REGISTERS_H
#define _AWUI_EMULATION_MASTERSYSTEM_REGISTERS_H

#include <stdint.h>

namespace awui {
	namespace Emulation {
		namespace MasterSystem {
			class Registers {
				private:
					uint8_t _a;
					uint8_t _f;
					uint16_t _bc;
					uint16_t _de;
					uint16_t _hl;
					uint8_t _a_;
					uint8_t _f_;
					uint16_t _bc_;
					uint16_t _de_;
					uint16_t _hl_;
					uint16_t _ix;
					uint16_t _iy;
					uint16_t _pc;
					uint16_t _sp;
					bool _iff1;
					bool _iff2;

				public:
					Registers();
					~Registers();

					void Clear();
					void Alternate();

					void SetSP(uint16_t value);
					uint16_t GetSP() const;

					uint16_t GetPC() const;
					void IncPC();
					void IncPC(uint8_t value);
					void SetPC(uint16_t value);

					void SetIFF1(bool mode);
					void SetIFF2(bool mode);

					void SetA(uint8_t value);
					void SetF(uint8_t value);
					uint8_t GetA() const;
					uint8_t GetF() const;

					void SetBC(uint16_t value);
					void SetB(uint8_t value);
					void SetC(uint8_t value);
					uint16_t GetBC() const;
					uint8_t GetB() const;
					uint8_t GetC() const;

					void SetDE(uint16_t value);
					void SetD(uint8_t value);
					void SetE(uint8_t value);
					uint16_t GetDE() const;
					uint8_t GetD() const;
					uint8_t GetE() const;

					void SetHL(uint16_t value);
					void SetH(uint8_t value);
					void SetL(uint8_t value);
					uint16_t GetHL() const;
					uint8_t GetH() const;
					uint8_t GetL() const;

					void SetIX(uint16_t value);
					uint16_t GetIX() const;

					void SetIY(uint16_t value);
					uint16_t GetIY() const;
			};
		}
	}
}

#endif