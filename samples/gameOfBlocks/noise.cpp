/**
 * samples/gameOfBlocks/noise.cpp
 *
 * Copyright (C) 2026 Borja Sánchez Zamorano
 */

#include "noise.h"

#include <cmath>
#include <string>

namespace {
	// El >>> 0 de JavaScript: la parte entera, módulo 2^32
	double ToUint32(double value) {
		return fmod(trunc(value), 4294967296.0);
	}

	// La función de mezcla de Alea (Mash 0.9): cada llamada sigue desde donde quedó la anterior
	class Mash {
	  private:
		double m_n = 0xefc8249d;

	  public:
		double operator()(const std::string &data) {
			for (char character : data) {
				m_n += (unsigned char) character;
				double h = 0.02519603282416938 * m_n;
				m_n = ToUint32(h);
				h -= m_n;
				h *= m_n;
				m_n = ToUint32(h);
				h -= m_n;
				m_n += h * 4294967296.0; // 2^32
			}

			return ToUint32(m_n) * 2.3283064365386963e-10; // 2^-32
		}
	};

	const double F2 = 0.5 * (sqrt(3.0) - 1.0);
	const double G2 = (3.0 - sqrt(3.0)) / 6.0;

	const double Grad2[24] = {1, 1, -1, 1, 1, -1, -1, -1, 1, 0, -1, 0, 1, 0, -1, 0, 0, 1, 0, -1, 0, 1, 0, -1};
} // namespace

Alea::Alea(int seed) {
	Mash mash;
	m_c = 1;
	m_s0 = mash(" ");
	m_s1 = mash(" ");
	m_s2 = mash(" ");

	std::string text = std::to_string(seed);
	m_s0 -= mash(text);
	if (m_s0 < 0)
		m_s0 += 1;
	m_s1 -= mash(text);
	if (m_s1 < 0)
		m_s1 += 1;
	m_s2 -= mash(text);
	if (m_s2 < 0)
		m_s2 += 1;
}

double Alea::Next() {
	double t = 2091639 * m_s0 + m_c * 2.3283064365386963e-10; // 2^-32
	m_s0 = m_s1;
	m_s1 = m_s2;
	m_c = trunc(t);
	m_s2 = t - m_c;
	return m_s2;
}

SimplexNoise2D::SimplexNoise2D(Alea &random) {
	for (int i = 0; i < 256; i++)
		m_perm[i] = (uint8_t) i;

	for (int i = 0; i < 255; i++) {
		int r = i + (int) (random.Next() * (256 - i));
		uint8_t aux = m_perm[i];
		m_perm[i] = m_perm[r];
		m_perm[r] = aux;
	}

	for (int i = 256; i < 512; i++)
		m_perm[i] = m_perm[i - 256];

	for (int i = 0; i < 512; i++) {
		m_gradX[i] = Grad2[(m_perm[i] % 12) * 2];
		m_gradY[i] = Grad2[(m_perm[i] % 12) * 2 + 1];
	}
}

double SimplexNoise2D::Noise(double x, double y) const {
	double n0 = 0, n1 = 0, n2 = 0; // Lo que aporta cada esquina del triángulo

	// La celda (un triángulo equilátero) en la que cae el punto
	double s = (x + y) * F2;
	int i = (int) floor(x + s);
	int j = (int) floor(y + s);
	double t = (i + j) * G2;
	double x0 = x - (i - t);
	double y0 = y - (j - t);

	int i1, j1;
	if (x0 > y0) {
		i1 = 1;
		j1 = 0;
	} else {
		i1 = 0;
		j1 = 1;
	}

	double x1 = x0 - i1 + G2;
	double y1 = y0 - j1 + G2;
	double x2 = x0 - 1.0 + 2.0 * G2;
	double y2 = y0 - 1.0 + 2.0 * G2;

	int ii = i & 255;
	int jj = j & 255;

	double t0 = 0.5 - x0 * x0 - y0 * y0;
	if (t0 >= 0) {
		int gi = ii + m_perm[jj];
		t0 *= t0;
		n0 = t0 * t0 * (m_gradX[gi] * x0 + m_gradY[gi] * y0);
	}

	double t1 = 0.5 - x1 * x1 - y1 * y1;
	if (t1 >= 0) {
		int gi = ii + i1 + m_perm[jj + j1];
		t1 *= t1;
		n1 = t1 * t1 * (m_gradX[gi] * x1 + m_gradY[gi] * y1);
	}

	double t2 = 0.5 - x2 * x2 - y2 * y2;
	if (t2 >= 0) {
		int gi = ii + 1 + m_perm[jj + 1];
		t2 *= t2;
		n2 = t2 * t2 * (m_gradX[gi] * x2 + m_gradY[gi] * y2);
	}

	return 70.0 * (n0 + n1 + n2);
}
