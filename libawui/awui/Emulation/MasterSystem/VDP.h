#pragma once

#include <stdint.h>

// Segun documentacion
#define LEFTBORDER 13
#define RIGHTBORDER 15

// Comparado con la Master System
// #define LEFTBORDER 1
// #define RIGHTBORDER 12

namespace awui::Emulation::MasterSystem {
	class Motherboard;
	enum {
		SPRITE_16x16 = 1,
		SPRITE_8x16 = 2,
		SPRITE_8x8 = 3,
	};

	class VDP {
	  private:
		struct saveData {
			uint16_t _width;
			uint16_t _height;
			uint16_t _line;
			uint16_t _col;
			uint16_t _baseAddress;
			uint16_t _dataByte;
			uint16_t _address;
			uint8_t _portState;
			uint8_t _readbuffer;
			uint8_t _verticalScroll;
			uint8_t _horizontalScroll; // Registro 8 fijado al empezar cada línea
			uint8_t _hcounterLatch;	// Contador horizontal capturado (puerto 0x7F)
			uint8_t _status;
			uint8_t _spriteSize;
			uint8_t _lineCounter;
			uint8_t _lineSprites[64]; // Sprites que se dibujan en la línea actual, en orden de la SAT
			uint8_t _lineSpriteCount;
			bool _controlMode : 1;
			bool _lineInterruptPending : 1;
			bool _ntsc : 1;
			bool _showBorder : 1;
			bool _visible : 1;
			uint8_t _cram[64]; // Master System: 32 colores de 1 byte. Game Gear: 32 colores de 2 bytes
			uint8_t _cramLatch; // Game Gear: byte par pendiente de escribir en la CRAM
			uint8_t _registers[11];
			uint8_t _vram[16384];
			uint16_t _sizeData;
			uint16_t _data[(256 + LEFTBORDER + RIGHTBORDER) * 294]; // Color 12 bits (0x0BGR). 294 parece que es el maximo en alto
		} d;

		// No salvable
		Motherboard *_cpu;
		bool _spriteLimit;
		bool _gameGear;

		uint8_t NTSCx192[262];
		uint8_t NTSCx224[262];
		uint8_t NTSCx240[262];
		uint8_t PALx192[313];
		uint8_t PALx224[313];
		uint8_t PALx240[313];
		uint8_t HORSYNC[342];

		uint8_t GetStatus(bool resetStatus = true);
		void UpdateAllRegisters();

		void ResetVideo();
		void SetHeight(uint16_t height);
		void CalcNextPixel(uint16_t *col, uint16_t *line, bool *hsync, bool *vsync);
		bool IsVSYNC(uint16_t line) const;

		uint8_t GetSpriteColor(uint16_t sprite, int x, int y, bool flipx, bool flipy, bool otherPalete, bool doble) const;
		void EvaluateSprites();
		void EvaluateLegacySprites();
		uint16_t GetLegacyPixel();
		inline bool IsMode4() const { return (this->d._registers[0] & 0x04) != 0; }
		bool GetSpritePixel(uint8_t *color);
		uint16_t GetBackgroundPixel(uint16_t sprite, int16_t x, int16_t y, bool flipx, bool flipy, bool otherPalete, bool bgPriority, bool hasSprite, uint8_t spriteColor) const;

		void OnTickBorder();
		void WriteControlByte(uint8_t value);
		void WriteDataByte(uint8_t value);
		uint16_t GetBackdropColor() const;
		uint16_t GetWidth() const;
		uint16_t GetHeight() const;
		uint16_t GetBufferWidth() const;
		uint16_t GetBufferHeight() const;

	  public:
		VDP(Motherboard *cpu);
		virtual ~VDP();

		bool OnTick(uint32_t counter);

		void WriteByte(uint8_t port, uint8_t value);
		uint8_t ReadByte(uint8_t port);

		void Reset();

		const uint8_t *GetColors() const;
		uint8_t *GetVram();

		void SetNTSC();
		void SetPAL();
		bool GetNTSC() const;
		bool GetPAL() const;
		uint16_t GetTotalWidth() const;
		uint16_t GetTotalHeight() const;
		uint16_t GetVisualWidth() const;
		uint16_t GetVisualHeight() const;
		uint16_t GetActiveTop() const;
		uint16_t GetActiveLeft() const;
		uint16_t GetBorderBottom() const;

		// Color del píxel en 12 bits: 0x0BGR, 4 bits por componente
		uint16_t GetPixel(uint16_t x, uint16_t y) const;
		// Color i (0-31) de la paleta en 12 bits: 0x0BGR
		uint16_t GetColor(uint8_t index) const;

		// Game Gear: paleta de 12 bits y ventana visible de 160x144 en el centro de la pantalla
		void SetGameGear(bool mode);
		inline bool IsGameGear() const { return _gameGear; }

		bool GetShowBorder() const;

		// Límite de 8 sprites por línea como en el hardware. Desactivarlo evita el parpadeo,
		// pero el flag de desbordamiento se sigue activando igual.
		inline void SetSpriteLimit(bool mode) { _spriteLimit = mode; }
		inline bool GetSpriteLimit() const { return _spriteLimit; }
		void SetShowBorder(bool mode);

		void Clear();

		bool IsIRQ() const;

		// Captura el contador horizontal (al subir TH en los puertos de mando)
		void LatchHCounter();

		uint16_t GetLine() const;
		uint16_t GetColumn() const;

		uint16_t GetBackColor() const;

		static int GetSaveSize();

		void LoadState(uint8_t *data);
		void SaveState(uint8_t *data);
	};
} // namespace awui::Emulation::MasterSystem
