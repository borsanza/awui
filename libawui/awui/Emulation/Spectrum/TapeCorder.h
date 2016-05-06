#ifndef _AWUI_EMULATION_SPECTRUM_TAPECORDER_H
#define _AWUI_EMULATION_SPECTRUM_TAPECORDER_H

#include <awui/Object.h>

#include <stdint.h>

namespace awui {
	namespace Collections {
		class ArrayList;
	}

	using namespace Collections;

	namespace Emulation {
		namespace Spectrum {
			class TapeBlock : public awui::Object {
				private:
					uint8_t * _data;
					int _size;

				public:
					TapeBlock(int size);
					~TapeBlock();

					void SetByte(int pos, uint8_t value);
					uint8_t GetByte(int pos);
					int GetLength();
			};

			class TapeCorder {
				private:
					ArrayList * _list;
					int _posByte;
					int _posBit;
					int _block;
					int _state;
					int32_t _cycle;
					bool _playing;

					void (*_finishCassetteCB) (void *);
					void * _finishCassetteDataCB;

					void Clear();

				public:
					TapeCorder();
					~TapeCorder();

					void LoadFile(const String file);

					void Rewind();
					void Play();
					uint32_t GetNext();

					void SetFinishCassetteCB(void (* fun)(void *), void * data);
			};
		}
	}
}

#endif
