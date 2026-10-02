/**
 * awui/Emulation/Common/RewindBuffer.cpp
 *
 * Copyright (C) 2026 Borja Sánchez Zamorano
 */

#include "RewindBuffer.h"

#include <cstring>

using namespace awui::Emulation::Common;

// Formato de un cambio comprimido: repeticiones de [ceros][n][n bytes], con los dos números en varint (LEB128).
// Los bytes son el XOR entre dos estados seguidos: aplicarlo sobre uno da el otro, en los dos sentidos

static void WriteVarint(std::vector<uint8_t> &out, size_t value) {
	while (value >= 0x80) {
		out.push_back((uint8_t) (value | 0x80));
		value >>= 7;
	}
	out.push_back((uint8_t) value);
}

static size_t ReadVarint(const uint8_t *&p) {
	size_t value = 0;
	int shift = 0;
	uint8_t b;
	do {
		b = *p++;
		value |= (size_t) (b & 0x7F) << shift;
		shift += 7;
	} while (b & 0x80);

	return value;
}

RewindBuffer::RewindBuffer(size_t stateSize, size_t maxBytes) {
	m_stateSize = stateSize;
	m_maxBytes = maxBytes;
	m_current.resize(stateSize);
	Clear();
}

void RewindBuffer::Clear() {
	m_back.clear();
	m_bytes = 0;
	m_hasCurrent = false;
}

void RewindBuffer::Compress(const uint8_t *state, std::vector<uint8_t> &out) const {
	const uint8_t *current = m_current.data();
	size_t i = 0;
	while (i < m_stateSize) {
		size_t zeros = i;
		while ((i < m_stateSize) && (state[i] == current[i]))
			i++;
		zeros = i - zeros;

		if (i == m_stateSize) {
			// Ceros hasta el final: no hace falta escribirlos
			break;
		}

		// Bytes distintos; un tramo corto de iguales en medio sale más barato dentro que partiéndolo
		size_t start = i;
		size_t same = 0;
		while (i < m_stateSize) {
			if (state[i] == current[i]) {
				if (++same >= 4)
					break;
			} else {
				same = 0;
			}
			i++;
		}

		size_t end = (same >= 4) ? (i - (same - 1)) : i;
		WriteVarint(out, zeros);
		WriteVarint(out, end - start);
		for (size_t j = start; j < end; j++)
			out.push_back(state[j] ^ current[j]);

		i = end;
	}
}

void RewindBuffer::ApplyDelta(const std::vector<uint8_t> &delta) {
	const uint8_t *p = delta.data();
	const uint8_t *endData = p + delta.size();
	uint8_t *current = m_current.data();
	size_t pos = 0;
	while (p < endData) {
		pos += ReadVarint(p);
		size_t count = ReadVarint(p);
		for (size_t j = 0; j < count; j++)
			current[pos + j] ^= p[j];
		p += count;
		pos += count;
	}
}

void RewindBuffer::Push(const uint8_t *state) {
	if (!m_hasCurrent) {
		memcpy(m_current.data(), state, m_stateSize);
		m_hasCurrent = true;
		return;
	}

	std::vector<uint8_t> delta;
	Compress(state, delta);
	delta.shrink_to_fit();
	m_bytes += delta.size();
	m_back.push_back(std::move(delta));
	memcpy(m_current.data(), state, m_stateSize);

	while ((m_bytes > m_maxBytes) && !m_back.empty()) {
		m_bytes -= m_back.front().size();
		m_back.pop_front();
	}
}

bool RewindBuffer::Back(uint8_t *state) {
	if (m_back.empty())
		return false;

	ApplyDelta(m_back.back());
	m_bytes -= m_back.back().size();
	m_back.pop_back();
	memcpy(state, m_current.data(), m_stateSize);
	return true;
}
