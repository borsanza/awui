/*
 * awui/Emulation/Spectrum/TapeCorder.cpp
 *
 * Copyright (C) 2016 Borja Sánchez Zamorano
 */

#include "TapeCorder.h"

#include <awui/Collections/ArrayList.h>
#include <awui/Emulation/Common/Word.h>
#include <awui/IO/FileStream.h>

#include <stdlib.h>

using namespace awui::Collections;
using namespace awui::Emulation::Spectrum;
using namespace awui::IO;

TapeCorder::TapeCorder() {
	this->_finishCassetteCB = NULL;
	this->_finishCassetteDataCB = NULL;
	this->_cycle = 0;
	this->_state = 0;
	this->_block = 0;
	this->_posBit = 0;
	this->_posByte = 0;
	this->_list = 0;
	this->_playing = false;
	this->Rewind();
}

TapeCorder::~TapeCorder() {
	this->Clear();
}

void TapeCorder::Clear() {
	if (this->_list) {
		for (int i = 0; i < this->_list->GetCount(); i++) {
			TapeBlock *block = (TapeBlock *) this->_list->Get(i);
			delete block;
		}

		delete this->_list;
	}
}

void TapeCorder::LoadFile(const String fileParam) {
	this->Clear();
	this->_list = new ArrayList();

	FileStream *file = new FileStream(fileParam, FileMode::Open, FileAccess::Read);

	Word blocks;
	int state = 0;

	int cont;
	TapeBlock *block;
	for (unsigned int i = 0; i < file->GetLength(); i++) {
		uint8_t data = file->ReadByte();
		switch (state) {
			case 0:
				state++;
				blocks.L = data;
				break;
			case 1:
				state++;
				blocks.H = data;
				block = new TapeBlock(blocks.W);
				this->_list->Add(block);
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

	file->Close();
	delete file;

	this->Rewind();
}

void TapeCorder::Play() {
	this->_playing = true;
}

void TapeCorder::Rewind() {
	this->_posByte = 0;
	this->_posBit = 0;
	this->_state = 0;
	this->_cycle = 8063;
	this->_block = 0;
}

uint32_t TapeCorder::GetNext() {
	if (!this->_playing)
		return -1;

	if (!this->_list)
		return -1;

	// if (this->_state != 8) printf("%d\n", this->_state);

	switch (this->_state) {
		case 0: // Carga lenta
			this->_cycle--;
			if (this->_cycle == 0)
				this->_state = 1;

			return 2168;
		case 1:
			this->_state = 2;
			return 667;
		case 2:
			this->_state = 3;
			return 735;

		case 3: { // Mandar Datos
			TapeBlock *tapeBlock = (TapeBlock *) this->_list->Get(this->_block);
			uint8_t data = tapeBlock->GetByte(this->_posByte);
			data = (data >> (7 - this->_posBit)) & 0x01;
			this->_state = 4;
			if (data == 0)
				return 855;

			return 1710;
		}

		case 4: { // Mandar Datos
			TapeBlock *tapeBlock = (TapeBlock *) this->_list->Get(this->_block);
			uint8_t data = tapeBlock->GetByte(this->_posByte);
			data = (data >> (7 - this->_posBit)) & 0x01;
			this->_posBit++;
			this->_state = 3;

			if (this->_posBit == 8) {
				this->_posBit = 0;
				this->_posByte++;

				if (this->_posByte == tapeBlock->GetLength()) {
					this->_state = 5;
					this->_posByte = 0;
					this->_block++;
				}
			}

			if (data == 0)
				return 855;

			return 1710;
		}

		case 5: // Mini Pausa
			if (this->_list && (this->_block >= this->_list->GetCount()))
				this->_state = 8;
			else
				this->_state = 6;
			return 945;
		case 6: // Pausa
			this->_state = 7;
			this->_cycle = 3223;
			return 3462773;
		case 7: // Carga mas rapida
			this->_cycle--;
			if (this->_cycle == 0)
				this->_state = 1;

			return 2168;
		case 8: // Fin
			if (this->_finishCassetteCB)
				this->_finishCassetteCB(this->_finishCassetteDataCB);
			this->_state = 9;
			break;
		case 9:
			break;
	}

	return -1;
}

void TapeCorder::SetFinishCassetteCB(void (*fun)(void *), void *data) {
	this->_finishCassetteCB = fun;
	this->_finishCassetteDataCB = data;
}

/******************************************************************************/

TapeBlock::TapeBlock(int size) {
	this->_data = (uint8_t *) malloc(size);
	this->_size = size;
}

TapeBlock::~TapeBlock() {
	free(this->_data);
}

void TapeBlock::SetByte(int pos, uint8_t value) {
	this->_data[pos] = value;
}

uint8_t TapeBlock::GetByte(int pos) {
	return this->_data[pos];
}

int TapeBlock::GetLength() {
	return this->_size;
}
