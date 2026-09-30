/*
 * awui/Emulation/MasterSystem/VDP.cpp
 *
 * Copyright (C) 2014 Borja Sánchez Zamorano
 */

#include "VDP.h"

#include <assert.h>
#include <awui/Emulation/MasterSystem/Motherboard.h>
#include <string.h>

using namespace awui::Emulation::MasterSystem;

// Paleta fija del TMS9918 (modos de la SG-1000) en 12 bits: 0x0BGR
static const uint16_t TMS_PALETTE[16] = {
	0x000, 0x000, 0x4C2, 0x7D6, 0xE55, 0xF77, 0x55C, 0xEE4, 0x55F, 0x77F, 0x5BC, 0x8CE, 0x3A2, 0xB5C, 0xCCC, 0xFFF,
};

/*
 * bit 7: VSync flag, set at the beginning of each VSync impulse
 *     6: Line interruot flag: set when a line interrupt is generated
 *        (see the VDP section for details on line interrupts)
 *     5: Sprite collision flag: set when two sprites overlap
 *     4: ?
 *     3: ?
 *     2: ?
 *     1: ?
 *     0: ?
 */

VDP::VDP(Motherboard *cpu) {
	m_saveData.width = 256;
	m_saveData.height = 192;
	m_saveData.ntsc = true;
	m_saveData.showBorder = false;
	ResetVideo();

	m_cpu = cpu;
	m_saveData.line = 0;
	m_saveData.col = 0;
	m_saveData.lineInterruptPending = false;
	m_saveData.lineCounter = 0xFF;
	m_saveData.verticalScroll = 0;
	m_saveData.horizontalScroll = 0;
	m_saveData.hcounterLatch = 0;
	m_saveData.lineSpriteCount = 0;
	m_spriteLimit = true;
	m_gameGear = false;

	m_saveData.status = 0x1F;
	m_saveData.address = 0;
	m_saveData.portState = 0xFF;
	m_saveData.baseAddress = 0;

	m_saveData.controlMode = false;

	// Rellenando array de valores para el vertical
	for (int i = 0; i < 262; i++) {
		m_vCounterNtsc192[i] = i <= 0xDA ? i : i - 0xDA + 0xD5 - 1; // 00-DA, D5-FF
		m_vCounterNtsc224[i] = i <= 0xEA ? i : i - 0xEA + 0xE5 - 1; // 00-EA, E5-FF
		m_vCounterNtsc240[i] = i <= 0xFF ? i : i - 0xFF + 0x00 - 1; // 00-FF, 00-06
	}

	for (int i = 0; i < 313; i++) {
		m_vCounterPal192[i] = (i <= 0xF2) ? i : i - 0x39;								// 00-F2, BA-FF
		m_vCounterPal224[i] = (i <= 0xFF) ? i : ((i <= 0x102) ? i - 0x100 : i - 0x39); // 00-FF, 00-02, CA-FF
		m_vCounterPal240[i] = (i <= 0xFF) ? i : ((i <= 0x10A) ? i - 0x100 : i - 0x39); // 00-FF, 00-0A, D2-FF
	}

	// Contador horizontal: avanza cada 2 píxeles. 0x00-0x93 desde el primer píxel visible
	// (columnas 0-295) y salta a 0xE9-0xFF (columnas 296-341)
	for (int i = 0; i < 342; i++)
		m_hCounter[i] = (i < 296) ? (i >> 1) : (0xE9 + ((i - 296) >> 1));

	Reset();
}

VDP::~VDP() {
}

void VDP::Reset() {
	Clear();
	memset(m_saveData.vram, 0, 16384 * sizeof(uint8_t));

	uint8_t values[] = {0x36, 0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0x00, 0x00, 0x00, 0xFF};
	for (uint8_t i = 0; i <= 10; i++)
		m_saveData.registers[i] = values[i];

	for (uint8_t i = 0; i < 64; i++)
		m_saveData.cram[i] = 0;
	m_saveData.cramLatch = 0;

	UpdateAllRegisters();
}

void VDP::Clear() {
	memset(m_saveData.data, 0, m_saveData.sizeData * sizeof(uint16_t));
}

const uint8_t *VDP::GetColors() const {
	return m_saveData.cram;
}

uint8_t *VDP::GetVram() {
	return m_saveData.vram;
}

bool VDP::GetShowBorder() const {
	return m_saveData.showBorder;
}

uint16_t VDP::GetLine() const {
	return m_saveData.line;
}

uint16_t VDP::GetColumn() const {
	return m_saveData.col;
}

void VDP::SetShowBorder(bool mode) {
	// La Game Gear no tiene borde: solo se ve el centro de la pantalla
	if (m_gameGear)
		mode = false;

	if (m_saveData.showBorder != mode) {
		m_saveData.showBorder = mode;
		ResetVideo();
	}
}

void VDP::ResetVideo() {
	m_saveData.sizeData = GetBufferWidth() * GetBufferHeight();
}

void VDP::SetGameGear(bool mode) {
	m_gameGear = mode;
	if (mode)
		SetShowBorder(false);
}

uint16_t VDP::GetColor(uint8_t index) const {
	index &= 0x1F;

	if (m_gameGear) {
		// 2 bytes por color: GGGGRRRR ----BBBB
		return m_saveData.cram[index * 2] | ((m_saveData.cram[(index * 2) + 1] & 0x0F) << 8);
	}

	// 1 byte por color: --BBGGRR. Cada componente de 2 bits pasa a 4 bits (x5: 0, 5, 10, 15)
	uint8_t c = m_saveData.cram[index];
	return ((c & 0x03) * 5) | ((((c >> 2) & 0x03) * 5) << 4) | ((((c >> 4) & 0x03) * 5) << 8);
}

void VDP::SetNTSC() {
	if (!m_saveData.ntsc) {
		m_saveData.ntsc = true;
		ResetVideo();
	}
}

void VDP::SetPAL() {
	if (m_saveData.ntsc) {
		m_saveData.ntsc = false;
		ResetVideo();
	}
}

bool VDP::GetNTSC() const {
	return m_saveData.ntsc;
}

bool VDP::GetPAL() const {
	return !m_saveData.ntsc;
}

uint16_t VDP::GetWidth() const {
	return m_saveData.width;
}

uint16_t VDP::GetHeight() const {
	return m_saveData.height;
}

void VDP::SetHeight(uint16_t height) {
	if (height != m_saveData.height) {
		m_saveData.height = height;
		ResetVideo();
	}
}

uint16_t VDP::GetTotalWidth() const {
	return 342;
}

uint16_t VDP::GetVisualWidth() const {
	if (m_gameGear)
		return 160;

	return GetBufferWidth();
}

uint16_t VDP::GetVisualHeight() const {
	if (m_gameGear)
		return 144;

	return GetBufferHeight();
}

uint16_t VDP::GetBufferWidth() const {
	if (m_saveData.showBorder)
		return 256 + LEFTBORDER + RIGHTBORDER;

	return GetWidth();
}

uint16_t VDP::GetBufferHeight() const {
	if (m_saveData.showBorder) {
		if (GetNTSC())
			return 243;
		else
			return 294;
	}

	return GetHeight();
}

uint16_t VDP::GetActiveLeft() const {
	if (m_saveData.showBorder)
		return LEFTBORDER;

	return 0;
}

uint16_t VDP::GetBorderBottom() const {
	if (m_saveData.showBorder)
		return GetBufferHeight() - (GetHeight() + GetActiveTop());

	return 0;
}

uint16_t VDP::GetActiveTop() const {
	if (m_saveData.showBorder) {
		if (GetNTSC()) {
			switch (m_saveData.height) {
				case 192:
					return 27;
				case 224:
					return 11;
				case 240:
					return 2;
			}
		} else {
			switch (m_saveData.height) {
				case 192:
					return 54;
				case 224:
					return 38;
				case 240:
					return 30;
			}
		}
	}

	return 0;
}

uint16_t VDP::GetTotalHeight() const {
	return (m_saveData.ntsc ? 262 : 313);
}

uint16_t VDP::GetPixel(uint16_t x, uint16_t y) const {
	// La Game Gear muestra 160x144 del centro de la pantalla
	if (m_gameGear) {
		x += (GetWidth() - 160) / 2;
		y += (GetHeight() - 144) / 2;
	}

	return m_saveData.data[(y * GetBufferWidth()) + x];
}

bool VDP::IsVSYNC(uint16_t line) const {
	if (GetNTSC()) {
		switch (m_saveData.height) {
			// 192    Active display
			// 24     Bottom border
			// 3      Bottom blanking
			// 3      Vertical blanking
			case 192:
				return 219 == line;

			// 224    Active display
			// 8      Bottom border
			// 3      Bottom blanking
			// 3      Vertical blanking
			case 224:
				return 235 == line;

			// 240    Active display
			case 240:
				// Segun documentacion no funciona en maquinas reales
				// assert(0);
				return 240 == line;
		}
	} else {
		switch (m_saveData.height) {
			// 192    Active display
			// 48     Bottom border
			// 3      Bottom blanking
			// 3      Vertical blanking
			case 192:
				return 249 == line; // Confirmado con la Master System

			// 224    Active display
			// 32     Bottom border
			// 3      Bottom blanking
			// 3      Vertical blanking
			case 224:
				return 281 == line;

			// 240    Active display
			// 24     Bottom border
			// 3      Bottom blanking
			// 3      Vertical blanking
			case 240:
				return 297 == line;
		}
	}

	assert(0);
	return false;
}

void VDP::CalcNextPixel(uint16_t *col, uint16_t *line, bool *hsync, bool *vsync) {
	*hsync = false;
	*vsync = false;

	(*col)++;
	if (*col == GetTotalWidth()) {
		*col = 0;
		(*line)++;
		if (*line == GetTotalHeight())
			*line = 0;
	}

	// Contador de líneas: se decrementa en las líneas 0..altura (incluida) y al pasar de 0 se recarga
	// con el registro 10 y queda pendiente la interrupción de línea. Fuera de esas líneas se recarga.
	// La interrupción de frame va con el flag VSync del estado (bit 7), ver IsIRQ()
	if (*col == 271) {
		if (*line <= m_saveData.height) {
			if (m_saveData.lineCounter == 0) {
				m_saveData.lineCounter = m_saveData.registers[10];
				m_saveData.lineInterruptPending = true;
			} else
				m_saveData.lineCounter--;
		} else
			m_saveData.lineCounter = m_saveData.registers[10];
	}

	// 256 Active Display +
	// 15 Right Border +
	// 8 Right Blank <- Suponemos que cambia al final del Right Blank
	// 26 HSync
	if (*col == 279) {
		*hsync = true;
		*vsync = IsVSYNC(*line);

		// Flag de interrupción de frame (bit 7 del estado): en la línea siguiente a la última visible
		// (VCounter 0xC1, 0xE1 o 0xF1 según la altura). Es independiente del VSync, que marca el fin del frame.
		if (*line == m_saveData.height + 1)
			m_saveData.status |= 0x80;
	}
}

void VDP::OnTickBorder() {
	int x = 0;
	int y = 0;
	bool draw = false;
	if ((m_saveData.col >= GetWidth()) || (m_saveData.line >= GetHeight())) {
		if (m_saveData.col >= (GetTotalWidth() - LEFTBORDER)) {
			draw = true;
			x = m_saveData.col - (GetTotalWidth() - LEFTBORDER);
		} else {
			if (m_saveData.col < GetWidth() + RIGHTBORDER) {
				draw = true;
				x = LEFTBORDER + m_saveData.col;
			}
		}

		if (m_saveData.line >= (GetTotalHeight() - GetActiveTop())) {
			draw = true;
			y = m_saveData.line - (GetTotalHeight() - GetActiveTop());
		} else {
			if (m_saveData.line < (GetHeight() + GetBorderBottom())) {
				draw = true;
				y = GetActiveTop() + m_saveData.line;
			}
		}
	}

	if (draw)
		m_saveData.data[x + (y * GetBufferWidth())] = GetBackdropColor();
}

// Background
uint8_t VDP::GetSpriteColor(uint16_t sprite, int x, int y, bool flipx, bool flipy, bool otherPalete, bool doble) const {
	if (doble) {
		x = x >> 1;
		y = y >> 1;
	}

	int64_t offset;
	if (flipy)
		offset = sprite * 32 + ((7 - y) * 4);
	else
		offset = sprite * 32 + (y * 4);

	uint8_t byte1 = m_saveData.vram[offset];
	uint8_t byte2 = m_saveData.vram[offset + 1];
	uint8_t byte3 = m_saveData.vram[offset + 2];
	uint8_t byte4 = m_saveData.vram[offset + 3];
	uint8_t mask, realX;

	if (flipx) {
		mask = 0x80 >> (7 - x);
		realX = x;
	} else {
		mask = 0x80 >> x;
		realX = 7 - x;
	}

	uint8_t c;

	c = (byte1 & mask) >> realX;
	c |= (((byte2 & mask) >> realX) << 1);
	c |= (((byte3 & mask) >> realX) << 2);
	c |= (((byte4 & mask) >> realX) << 3);
	if (otherPalete)
		c |= 16;

	// assert(c < 32);
	return c;
}

// Sprite
// Busca los sprites que tocan la línea actual. Como en el hardware, solo se dibujan los 8 primeros
// (en orden de la SAT); si hay más se activa el flag de desbordamiento (bit 6 del estado).
// En el modo de 192 líneas, una Y de 0xD0 termina la lista.
void VDP::EvaluateSprites() {
	uint8_t y = m_saveData.line;
	uint8_t height = (m_saveData.registers[1] & 0x2) ? 16 : 8;
	if (m_saveData.registers[1] & 0x1)
		height = height << 1;

	uint16_t base = ((uint16_t) m_saveData.registers[5] & 0x7E) << 7;

	int count = 0;
	bool overflow = false;
	int limit = m_spriteLimit ? 8 : 64;

	for (int n = 0; n < 64; n++) {
		int16_t sy = m_saveData.vram[base + n];

		if ((m_saveData.height == 192) && (sy == 0xD0))
			break;

		if (sy > (255 - height))
			sy -= 256;

		if ((y <= sy) || (y > (sy + height)))
			continue;

		if (count >= 8)
			overflow = true;

		if (count >= limit)
			break;

		m_saveData.lineSprites[count++] = n;
	}

	if (overflow)
		m_saveData.status |= 0x40;

	m_saveData.lineSpriteCount = count;
}

// Color del sprite en el píxel actual. Si dos sprites tienen un píxel opaco en el mismo sitio
// se activa el flag de colisión (bit 5 del estado); se dibuja el primero de la SAT.
bool VDP::GetSpritePixel(uint8_t *color) {
	int16_t x = m_saveData.col;
	uint8_t y = m_saveData.line;

	int offset = (m_saveData.registers[0] & 0x8) ? 8 : 0;

	uint8_t height = (m_saveData.registers[1] & 0x2) ? 16 : 8;
	uint8_t width = 8;

	bool doble = (m_saveData.registers[1] & 0x1);
	if (doble) {
		height = height << 1;
		width = width << 1;
	}

	uint16_t base = ((uint16_t) m_saveData.registers[5] & 0x7E) << 7;

	bool found = false;
	for (int i = 0; i < m_saveData.lineSpriteCount; i++) {
		int n = m_saveData.lineSprites[i];

		int16_t sx = m_saveData.vram[base + 128 + (n * 2)];
		sx -= offset;

		if ((x < sx) || (x >= (sx + width)))
			continue;

		int16_t sy = m_saveData.vram[base + n];
		if (sy > (255 - height))
			sy -= 256;

		uint16_t pattern = m_saveData.vram[base + 129 + (n * 2)];
		if (m_saveData.registers[6] & 0x4)
			pattern |= 0x100;

		// En sprites de 8x16 el hardware ignora el bit 0: se usan el tile par y el siguiente
		if (m_saveData.registers[1] & 0x2)
			pattern &= ~1;

		uint8_t c = GetSpriteColor(pattern, x - sx, y - sy - 1, false, false, true, doble);
		if ((c & 0xF) == 0)
			continue;

		if (found) {
			m_saveData.status |= 0x20;
			break;
		}

		*color = c;
		found = true;
	}

	return found;
}

// Sprites del TMS9918: 32 sprites de 4 bytes (Y, X, patrón, color). Como máximo 4 por línea;
// si hay un quinto se activa el bit 6 del estado y su número queda en los bits 0-4.
void VDP::EvaluateLegacySprites() {
	int line = m_saveData.line;
	int size = ((m_saveData.registers[1] & 0x02) ? 16 : 8) << (m_saveData.registers[1] & 0x01);
	uint16_t base = (m_saveData.registers[5] & 0x7F) << 7;

	int count = 0;
	for (int n = 0; n < 32; n++) {
		int sy = m_saveData.vram[base + (n * 4)];
		if (sy == 0xD0)
			break;

		if (sy >= 0xE0)
			sy -= 256;
		sy++;

		if ((line < sy) || (line >= sy + size))
			continue;

		if (count == 4) {
			if (!(m_saveData.status & 0x40))
				m_saveData.status = (m_saveData.status & 0xA0) | 0x40 | n;
			break;
		}

		m_saveData.lineSprites[count++] = n;
	}

	m_saveData.lineSpriteCount = count;
}

// Píxel en los modos del TMS9918: Graphics I, Graphics II, Text y Multicolor, con sus sprites
uint16_t VDP::GetLegacyPixel() {
	int x = m_saveData.col;
	int y = m_saveData.line;
	uint8_t *vram = m_saveData.vram;
	uint8_t *reg = m_saveData.registers;

	bool text = reg[1] & 0x10;
	bool multicolor = reg[1] & 0x08;
	bool graphics2 = reg[0] & 0x02;

	// Sprites (no hay en el modo texto). La colisión cuenta los píxeles del patrón aunque su color sea transparente.
	if (!text) {
		int sizeBase = (reg[1] & 0x02) ? 16 : 8;
		int magnify = reg[1] & 0x01;
		uint16_t base = (reg[5] & 0x7F) << 7;
		uint16_t patterns = (reg[6] & 0x07) << 11;
		bool hit = false;	// Algún sprite anterior tiene un píxel del patrón aquí
		bool found = false; // Ya hay un sprite con color visible
		uint8_t spriteColor = 0;

		for (int i = 0; i < m_saveData.lineSpriteCount; i++) {
			int n = m_saveData.lineSprites[i];
			int sy = vram[base + (n * 4)];
			if (sy >= 0xE0)
				sy -= 256;
			sy++;

			int sx = vram[base + (n * 4) + 1];
			uint8_t pattern = vram[base + (n * 4) + 2];
			uint8_t color = vram[base + (n * 4) + 3];
			if (color & 0x80)
				sx -= 32;

			int dx = x - sx;
			if ((dx < 0) || (dx >= (sizeBase << magnify)))
				continue;

			int dy = (y - sy) >> magnify;
			dx >>= magnify;

			uint16_t addr;
			if (sizeBase == 16)
				addr = patterns + ((pattern & 0xFC) * 8) + dy + ((dx & 8) ? 16 : 0);
			else
				addr = patterns + (pattern * 8) + dy;

			if (!(vram[addr & 0x3FFF] & (0x80 >> (dx & 7))))
				continue;

			// La colisión cuenta cualquier píxel del patrón; el color solo decide qué sprite se ve
			if (hit)
				m_saveData.status |= 0x20;
			hit = true;

			if (!found && (color & 0x0F)) {
				found = true;
				spriteColor = color & 0x0F;
			}
		}

		if (found)
			return TMS_PALETTE[spriteColor];
	}

	uint16_t names = (reg[2] & 0x0F) << 10;
	uint8_t c;

	if (text) {
		// 40 columnas de 6 píxeles, con 8 píxeles de borde a cada lado
		if ((x < 8) || (x >= 248))
			c = 0;
		else {
			int tx = (x - 8) / 6;
			uint8_t name = vram[names + ((y >> 3) * 40) + tx];
			uint8_t pattern = vram[((reg[4] & 0x07) << 11) + (name * 8) + (y & 7)];
			c = (pattern & (0x80 >> ((x - 8) % 6))) ? (reg[7] >> 4) : (reg[7] & 0x0F);
		}
	} else if (multicolor) {
		// Bloques de 4x4 píxeles con dos colores por byte
		uint8_t name = vram[names + ((y >> 3) * 32) + (x >> 3)];
		uint8_t b = vram[((reg[4] & 0x07) << 11) + (name * 8) + (((y >> 3) & 3) * 2) + ((y >> 2) & 1)];
		c = (x & 4) ? (b & 0x0F) : (b >> 4);
	} else {
		uint8_t name = vram[names + ((y >> 3) * 32) + (x >> 3)];
		int row = y & 7;
		uint16_t patternAddr, colorAddr;

		if (graphics2) {
			// Graphics II: la pantalla se divide en tres tercios de 256 tiles. Los registros 3 y 4 hacen de máscara.
			int index = ((y >> 6) << 8) + name;
			patternAddr = ((reg[4] & 0x04) << 11) + ((index & (((reg[4] & 0x03) << 8) | 0xFF)) << 3) + row;
			colorAddr = ((reg[3] & 0x80) << 6) + ((index & (((reg[3] & 0x7F) << 3) | 0x07)) << 3) + row;
		} else {
			// Graphics I: un byte de color por cada grupo de 8 tiles
			patternAddr = ((reg[4] & 0x07) << 11) + (name * 8) + row;
			colorAddr = (reg[3] << 6) + (name >> 3);
		}

		uint8_t pattern = vram[patternAddr & 0x3FFF];
		uint8_t colors = vram[colorAddr & 0x3FFF];
		c = (pattern & (0x80 >> (x & 7))) ? (colors >> 4) : (colors & 0x0F);
	}

	// El color 0 es transparente: se ve el color de fondo del registro 7
	if (c == 0)
		c = reg[7] & 0x0F;

	return TMS_PALETTE[c];
}

uint16_t VDP::GetBackgroundPixel(uint16_t sprite, int16_t x, int16_t y, bool flipx, bool flipy, bool otherPalete, bool bgPriority, bool hasSprite, uint8_t spriteColor) const {
	uint8_t color = GetSpriteColor(sprite, x, y, flipx, flipy, otherPalete, false);

	// El sprite se ve encima salvo que el tile tenga prioridad y su color no sea el transparente
	if (hasSprite && (!bgPriority || ((color & 0xF) == 0)))
		return GetColor(spriteColor);

	return GetColor(color);
}

bool VDP::OnTick(uint32_t counter) {
	bool ret = false;
	bool hsync, vsync;

	CalcNextPixel(&m_saveData.col, &m_saveData.line, &hsync, &vsync);
	if (vsync)
		ret = true;

	//	if (hsync)
	//		m_status |= 0x40;

	if (m_saveData.line == 0 && m_saveData.col == 0)
		m_saveData.verticalScroll = m_saveData.registers[9];

	// El scroll horizontal se fija al empezar cada línea: un cambio a mitad de línea se aplica en la siguiente
	if (m_saveData.col == 0)
		m_saveData.horizontalScroll = m_saveData.registers[8];

	if ((m_saveData.col == 0) && (m_saveData.line < m_saveData.height)) {
		if (!(m_saveData.registers[1] & 0x40))
			m_saveData.lineSpriteCount = 0;
		else if (IsMode4())
			EvaluateSprites();
		else
			EvaluateLegacySprites();
	}

	// Modos del TMS9918 (SG-1000): el registro 0 tiene M4 a 0
	if (!IsMode4() && (m_saveData.col < m_saveData.width) && (m_saveData.line < m_saveData.height)) {
		int32_t pos;
		if (m_saveData.showBorder)
			pos = m_saveData.col + GetActiveLeft() + ((m_saveData.line + GetActiveTop()) * GetBufferWidth());
		else
			pos = m_saveData.col + (m_saveData.line * GetBufferWidth());

		m_saveData.data[pos] = (m_saveData.registers[1] & 0x40) ? GetLegacyPixel() : GetBackdropColor();
	} else if ((m_saveData.col < m_saveData.width) && (m_saveData.line < m_saveData.height)) {
		int16_t col = m_saveData.col;
		int16_t line = m_saveData.line;
		bool vScroll = true;
		bool hScroll = true;
		bool black = false;

		if (((line >> 3) <= 1) && (m_saveData.registers[0] & 0x40))
			hScroll = false;

		if ((((col - (m_saveData.horizontalScroll & 0x7)) >> 3) >= 24) && (m_saveData.registers[0] & 0x80))
			vScroll = false;
		else if ((((col >> 3) >= 24) && (m_saveData.registers[0] & 0x80)) && !hScroll)
			vScroll = false;

		if (col < (m_saveData.horizontalScroll & 0x7) && hScroll && ((m_saveData.horizontalScroll & 0x7) != 0))
			black = true;

		if (hScroll) {
			col = col - m_saveData.horizontalScroll;
			while (col < 0)
				col += 256;
		}

		// La tabla de nombres tiene 28 filas en el modo de 192 líneas (el scroll da la vuelta en 224)
		// y 32 filas en los de 224/240 (da la vuelta en 256)
		if (vScroll) {
			line = line + m_saveData.verticalScroll;
			if (m_saveData.height == 192) {
				while (line >= 224)
					line -= 224;
			} else
				line &= 0xFF;
		}

		int32_t pos;
		if (m_saveData.showBorder)
			pos = m_saveData.col + GetActiveLeft() + ((m_saveData.line + GetActiveTop()) * GetBufferWidth());
		else
			pos = m_saveData.col + (m_saveData.line * GetBufferWidth());

		// Blank Display
		if (black || !(m_saveData.registers[1] & 0x40) || ((m_saveData.registers[0] & 0x20) && (m_saveData.col < 8))) {
			m_saveData.data[pos] = GetBackdropColor();
		} else {
			int32_t offset = m_saveData.baseAddress + ((line >> 3) * 64) + ((col >> 3) * 2);
			uint8_t byte1 = m_saveData.vram[offset];
			uint8_t byte2 = m_saveData.vram[offset + 1];
			uint16_t sprite = ((byte2 & 0x1) << 8) | byte1;
			bool flipx = byte2 & 2;
			bool flipy = byte2 & 4;
			bool otherPalette = byte2 & 8;
			bool priority = byte2 & 16;

			uint8_t spriteColor = 0;
			bool hasSprite = GetSpritePixel(&spriteColor);
			m_saveData.data[pos] = GetBackgroundPixel(sprite, col & 0x7, line & 0x7, flipx, flipy, otherPalette, priority, hasSprite, spriteColor);
		}
	} else {
		if (m_saveData.showBorder)
			OnTickBorder();
	}

	return ret;
}

/*
 * NTSC, 256x192   262 (3 vsync)
 * NTSC, 256x224   262 (3 vsync)
 * PAL, 256x192    313 (3 vsync)
 * PAL, 256x224    313 (3 vsync)
 * PAL, 256x240    313 (3 vsync)
 * Horizontal:     342 (26 hsync)
 */

uint8_t VDP::GetStatus(bool resetStatus) {
	uint8_t r = m_saveData.status;

	// Leer el estado limpia los bits 7, 6 y 5, la interrupción de línea pendiente
	// y reinicia la escritura en dos bytes del puerto de control
	if (resetStatus) {
		m_saveData.status &= 0x1F;
		m_saveData.lineInterruptPending = false;
		m_saveData.controlMode = false;
	}

	return r;
}

void VDP::UpdateAllRegisters() {
	// Register #0

	// Register #1
	m_saveData.visible = m_saveData.registers[0] & 0x40;

	// Los modos del TMS9918 siempre tienen 192 líneas
	if (!(m_saveData.registers[0] & 0x04))
		SetHeight(192);

	if (m_saveData.registers[0] & 0x04) {
		uint16_t height = 192;
		if (m_saveData.registers[0] & 0x02) {
			if (m_saveData.registers[1] & 0x10)
				height = 224;
			else if (m_saveData.registers[1] & 0x08)
				height = 240;
		}

		SetHeight(height);
	}

	if (m_saveData.registers[1] & 0x01) {
		if (m_saveData.registers[0] & 0x04)
			m_saveData.spriteSize = SPRITE_8x16;
		else
			m_saveData.spriteSize = SPRITE_16x16;
	} else
		m_saveData.spriteSize = SPRITE_8x8;

	// Register #2
	uint8_t code = (m_saveData.registers[2] >> 1) & 0x7;
	if ((m_saveData.height == 224) || (m_saveData.height == 240)) {
		code = code >> 1;
		m_saveData.baseAddress = (code * 0x1000) + 0x0700;
	} else {
		m_saveData.baseAddress = code * 0x0800;
	}
}

void VDP::WriteControlByte(uint8_t value) {
	if (!m_saveData.controlMode) {
		m_saveData.controlMode = true;
		m_saveData.address = (m_saveData.address & 0x3F00) | value;
		return;
	}

	m_saveData.address = (value & 0x3F) << 8 | (m_saveData.address & 0xFF);

	m_saveData.portState = value >> 6;

	switch (m_saveData.portState) {
		// A byte of VRAM is read from the location defined by the
		// address register and is stored in the read buffer. The
		// address register is incremented by one. Writes to the
		// data port go to VRAM.
		case 0:
			m_saveData.readbuffer = m_saveData.vram[m_saveData.address];
			m_saveData.address = (m_saveData.address + 1) & 0x3FFF;
			break;

		// Writes to the data port go to VRAM.
		case 1:
			break;

		// This value signifies a VDP register write, explained
		// below. Writes to the data port go to VRAM.
		case 2: {
			uint8_t pos = value & 0xF;
			if (pos < 11) {
				m_saveData.registers[pos] = m_saveData.address & 0xFF;
				UpdateAllRegisters();
			}
		} break;

		// Writes to the data port go to CRAM.
		case 3:
			break;
	}

	m_saveData.controlMode = false;
}

void VDP::WriteDataByte(uint8_t value) {
	// Cualquier acceso al puerto de datos reinicia la escritura en dos bytes del puerto de control
	m_saveData.controlMode = false;

	switch (m_saveData.portState) {
		case 0:
		case 1:
		case 2:
			m_saveData.vram[m_saveData.address] = value;
			m_saveData.readbuffer = value;
			break;
		case 3:
			if (m_gameGear) {
				// Game Gear: el byte par se guarda y al escribir el impar se actualizan los dos
				if (m_saveData.address & 1) {
					m_saveData.cram[(m_saveData.address & 0x3E)] = m_saveData.cramLatch;
					m_saveData.cram[(m_saveData.address & 0x3F)] = value;
				} else
					m_saveData.cramLatch = value;
			} else
				m_saveData.cram[m_saveData.address & 0x1F] = value;

			// La escritura en CRAM también deja el valor en el buffer de lectura
			m_saveData.readbuffer = value;
			break;
	}

	m_saveData.address = (m_saveData.address + 1) & 0x3FFF;
}

void VDP::WriteByte(uint8_t port, uint8_t value) {
	bool even = ((port & 1) == 0);

	if (port >= 0x40 && port <= 0x7F) {
		if (even) {
		} else {
		}
	}

	if (port >= 0x80 && port <= 0xBF) {
		if (even) {
			WriteDataByte(value);
		} else {
			WriteControlByte(value);
		}
	}
}

uint8_t VDP::ReadByte(uint8_t port) {
	bool even = ((port & 1) == 0);
	bool r = false;

	if (port >= 0x40 && port <= 0x7F) {
		r = true;
		if (even) {
			if (m_saveData.ntsc) {
				switch (m_saveData.height) {
					case 192:
						return m_vCounterNtsc192[m_saveData.line];
						break;
					case 224:
						return m_vCounterNtsc224[m_saveData.line];
						break;
					case 240:
						return m_vCounterNtsc240[m_saveData.line];
						break;
				}
			} else {
				switch (m_saveData.height) {
					case 192:
						return m_vCounterPal192[m_saveData.line];
						break;
					case 224:
						return m_vCounterPal224[m_saveData.line];
						break;
					case 240:
						return m_vCounterPal240[m_saveData.line];
						break;
				}
			}
		} else
			return m_saveData.hcounterLatch;
	}

	if (port >= 0x80 && port <= 0xBF) {
		r = true;
		if (even) {
			uint8_t ret;

			// Cualquier acceso al puerto de datos reinicia la escritura en dos bytes del puerto de control
			m_saveData.controlMode = false;

			// Siempre devuelve el buffer de lectura y lo rellena con la VRAM, sea cual sea el código
			// (con el código 3 tampoco se lee la CRAM)
			ret = m_saveData.readbuffer;
			m_saveData.readbuffer = m_saveData.vram[m_saveData.address];

			m_saveData.address = (m_saveData.address + 1) & 0x3FFF;

			return ret;
		} else {
			return GetStatus();
		}
	}

	if (!r) {
		printf("VDP::ReadByte(port = %.2X);\n", port);
		assert(0);
	}

	return 0;
}

// La línea IRQ se mantiene activa mientras haya una interrupción pendiente y habilitada
// (frame: bit 7 del estado + registro 1 bit 5, línea: pendiente + registro 0 bit 4)
bool VDP::IsIRQ() const {
	bool frame = (m_saveData.status & 0x80) && (m_saveData.registers[1] & 0x20);
	bool line = m_saveData.lineInterruptPending && (m_saveData.registers[0] & 0x10);

	return frame || line;
}

// Color de fondo y borde: el registro 7 elige uno de la segunda paleta (la de los sprites, colores 16-31)
// El puerto del contador horizontal devuelve el último valor capturado, no el contador en marcha
void VDP::LatchHCounter() {
	m_saveData.hcounterLatch = m_hCounter[m_saveData.col];
}

uint16_t VDP::GetBackdropColor() const {
	if (!IsMode4())
		return TMS_PALETTE[m_saveData.registers[7] & 0x0F];

	return GetColor(0x10 | (m_saveData.registers[7] & 0x0F));
}

uint16_t VDP::GetBackColor() const {
	return GetBackdropColor();
}

int VDP::GetSaveSize() {
	return sizeof(VDP::saveData);
}

void VDP::LoadState(uint8_t *data) {
	memcpy(&m_saveData, data, sizeof(VDP::saveData));
}

void VDP::SaveState(uint8_t *data) {
	memcpy(data, &m_saveData, sizeof(VDP::saveData));
}
