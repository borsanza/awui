#pragma once

#include <cstdint>

// Ruido para el terreno, igual que el de la versión web para que salga el mismo mundo: el generador de números
// aleatorios Alea (de Johannes Baagøe, el paquete "alea") y el ruido simplex en 2D del paquete "simplex-noise".
// Todo en double, como en JavaScript: con float cambiarían algunas alturas

// Números entre 0 y 1 a partir de una semilla. Alea(1) da la misma serie que alea(1) en JavaScript
class Alea {
  private:
	double m_s0, m_s1, m_s2;
	double m_c;

  public:
	Alea(int seed);

	double Next();
};

class SimplexNoise2D {
  private:
	uint8_t m_perm[512];
	double m_gradX[512];
	double m_gradY[512];

  public:
	// random: de donde sale la tabla de permutaciones
	SimplexNoise2D(Alea &random);

	// Entre -1 y 1
	double Noise(double x, double y) const;
};
