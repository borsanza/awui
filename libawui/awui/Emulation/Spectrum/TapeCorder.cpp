/*
 * awui/Emulation/Spectrum/TapeCorder.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "TapeCorder.h"

#include <awui/Emulation/Common/Word.h>
#include <awui/IO/File.h>
#include <vector>

#include <stdlib.h>

using namespace awui::Emulation::Spectrum;
using namespace awui::IO;

TapeCorder::TapeCorder() {
	m_finishCassetteCB = NULL;
	m_finishCassetteDataCB = NULL;
	m_cycle = 0;
	m_state = 0;
	m_block = 0;
	m_posBit = 0;
	m_posByte = 0;
	m_list = 0;
	m_playing = false;
	Rewind();
}

TapeCorder::~TapeCorder() {
	Clear();
}

void TapeCorder::Clear() {
	if (m_list) {
		for (TapeBlock *block : *m_list) {
			delete block;
		}

		delete m_list;
		m_list = NULL;
	}
}

void TapeCorder::LoadFile(const String fileParam) {
	Clear();
	m_list = new std::vector<TapeBlock *>();

	std::vector<uint8_t> bytes;
	if (!File::ReadAllBytes(fileParam, bytes))
		fprintf(stderr, "No se puede abrir el fichero: %s\n", fileParam.ToCharArray());

	Word blocks;
	int state = 0;

	int cont;
	TapeBlock *block;
	for (uint8_t data : bytes) {
		switch (state) {
			case 0:
				state++;
				blocks.L = data;
				break;
			case 1:
				blocks.H = data;
				// Un bloque de longitud 0 no tiene datos: se ignora (si no, la longitud daría la vuelta a 0xFFFF)
				if (blocks.W == 0) {
					state = 0;
					break;
				}

				state++;
				block = new TapeBlock(blocks.W);
				m_list->push_back(block);
				cont = 0;
				break;
			case 2:
				block->SetByte(cont, data);
				cont++;
				blocks.W--;
				if (blocks.W == 0)
					state = 0;
				break;
		}
	}

	Rewind();
}

void TapeCorder::Play() {
	m_playing = true;
}

void TapeCorder::Rewind() {
	m_posByte = 0;
	m_posBit = 0;
	m_state = 0;
	m_cycle = 8063;
	m_block = 0;
}

uint32_t TapeCorder::GetNext() {
	if (!m_playing)
		return -1;

	if (!m_list)
		return -1;

	// if (m_state != 8) printf("%d\n", m_state);

	switch (m_state) {
		case 0: // Carga lenta
			m_cycle--;
			if (m_cycle == 0)
				m_state = 1;

			return 2168;
		case 1:
			m_state = 2;
			return 667;
		case 2:
			m_state = 3;
			return 735;

		case 3: { // Mandar Datos
			// Cinta sin bloques: no hay nada que mandar
			if (m_block >= (int) m_list->size()) {
				m_state = 8;
				return 945;
			}

			TapeBlock *tapeBlock = (*m_list)[m_block];
			uint8_t data = tapeBlock->GetByte(m_posByte);
			data = (data >> (7 - m_posBit)) & 0x01;
			m_state = 4;
			if (data == 0)
				return 855;

			return 1710;
		}

		case 4: { // Mandar Datos
			TapeBlock *tapeBlock = (*m_list)[m_block];
			uint8_t data = tapeBlock->GetByte(m_posByte);
			data = (data >> (7 - m_posBit)) & 0x01;
			m_posBit++;
			m_state = 3;

			if (m_posBit == 8) {
				m_posBit = 0;
				m_posByte++;

				if (m_posByte == tapeBlock->GetLength()) {
					m_state = 5;
					m_posByte = 0;
					m_block++;
				}
			}

			if (data == 0)
				return 855;

			return 1710;
		}

		case 5: // Mini Pausa
			if (m_list && (m_block >= (int) m_list->size()))
				m_state = 8;
			else
				m_state = 6;
			return 945;
		case 6: // Pausa
			m_state = 7;
			m_cycle = 3223;
			return 3462773;
		case 7: // Carga mas rapida
			m_cycle--;
			if (m_cycle == 0)
				m_state = 1;

			return 2168;
		case 8: // Fin
			if (m_finishCassetteCB)
				m_finishCassetteCB(m_finishCassetteDataCB);
			m_state = 9;
			break;
		case 9:
			break;
	}

	return -1;
}

TapeBlock *TapeCorder::TakeNextBlock() {
	if (!m_list || (m_block >= (int) m_list->size()))
		return nullptr;

	TapeBlock *block = (*m_list)[m_block];
	m_block++;
	m_posByte = 0;
	m_posBit = 0;

	if (m_block >= (int) m_list->size()) {
		m_state = 9;
		m_playing = false;
		if (m_finishCassetteCB)
			m_finishCassetteCB(m_finishCassetteDataCB);
	} else {
		// Si luego se sigue a velocidad normal, la cinta está en el tono guía del siguiente bloque
		m_state = 7;
		m_cycle = 3223;
	}

	return block;
}

void TapeCorder::SetFinishCassetteCB(void (*fun)(void *), void *data) {
	m_finishCassetteCB = fun;
	m_finishCassetteDataCB = data;
}

/******************************************************************************/

TapeBlock::TapeBlock(int size) {
	// A cero: si el fichero está cortado, los bytes que faltan se leen como 0 y no como basura
	m_data = (uint8_t *) calloc(size, 1);
	m_size = size;
}

TapeBlock::~TapeBlock() {
	free(m_data);
}

void TapeBlock::SetByte(int pos, uint8_t value) {
	m_data[pos] = value;
}

uint8_t TapeBlock::GetByte(int pos) {
	return m_data[pos];
}

int TapeBlock::GetLength() const {
	return m_size;
}
