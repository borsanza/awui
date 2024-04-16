#include "Matrix4.h"

using namespace awui::GOB::Engine;

Matrix4::Matrix4() {
	for (int i = 0; i < 16; i++) {
		m[i] = 0;
	}

	m[0] = m[5] = m[10] = m[15] = 1;
}

float *Matrix4::operator[](int index) {
	return &m[index * 4];
}

const float *Matrix4::data() const {
	return m;
}
